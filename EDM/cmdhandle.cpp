#include "cmdhandle.h"
#include <math.h>

#define  EDM_PI 3.1415926


CmdHandle::CmdHandle(unsigned char bVectCmd,QString strCommand,DIGIT_CMD *pDigitCmd,DIGIT_CMD* pCmdDefault)
{
    if (!bVectCmd)
    {
        AnalyseCommand(strCommand,pDigitCmd,pCmdDefault);
    }
    else
    {
        AnalyseVectCommand(strCommand,pDigitCmd,pCmdDefault);
    }
}

CmdHandle::~CmdHandle()
{

}

void CmdHandle::AnalyseCommand(QString strCommand,DIGIT_CMD *pDigitCmd,DIGIT_CMD* pCmdDefault)
{
    int iIndex(-1);
    QString strCmdWay("");
    QString strCmdTemp("");

    memset(pDigitCmd,0,sizeof(DIGIT_CMD));
    pDigitCmd->iFreq = pCmdDefault->iFreq;
    pDigitCmd->enAim = pCmdDefault->enAim;
    pDigitCmd->enOrbit = pCmdDefault->enOrbit;
    pDigitCmd->enCoor = pCmdDefault->enCoor;

    strCommand = strCommand.trimmed().toUpper();

    iIndex = FindAndCalSpeed(strCommand);
    if (iIndex!= -1)
        strCommand = strCommand.left(iIndex);

    AnalyseCmdMode(strCommand,pDigitCmd);
    WriteCmd2Para(strCommand ,pDigitCmd);
}



void CmdHandle::AnalyseCmdMode(QString &str,DIGIT_CMD *pDigitCmd)
{
    static QString strAim[3] = {"G90","G91","G92"};
    static QString strOribit[4] = {"G00","G01","G02","G03"};
    static QString strCoord[6] = {"G54","G55","G56","G57","G58","G59"};

    QString strTmp,strCmdWay;
    int iLen;
    unsigned char bMatch;

    for (int i=0;i<3;i++)
    {
        bMatch = FALSE;
        str.trimmed();
        strTmp = str;
        iLen = strTmp.length();
        if (iLen<3)
        {
            return;
        }
        strCmdWay = strTmp.left(3);
        for (int j=0;j<3;j++)
        {
            if (strCmdWay == strAim[j])
            {
                pDigitCmd->enAim = (EDM_AIM_TYPE)j;
                str = strTmp.right(iLen-3);
                bMatch = TRUE;
                break;
            }
        }
        if (bMatch)
        {
            continue;
        }

        for (int j=0;j<4;j++)
        {
            if (strCmdWay == strOribit[j])
            {
                pDigitCmd->enOrbit = (EDM_ORBIT_TYPE)j;
                str = strTmp.right(iLen-3);
                bMatch = TRUE;
                break;
            }
        }
        if (bMatch)
        {
            continue;
        }

        for (int j=0;j<6;j++)
        {
            if (strCmdWay == strCoord[j])
            {
                pDigitCmd->enCoor = (EDM_COOR_TYPE)j;
                str = strTmp.right(iLen-3);
                bMatch = TRUE;
                break;
            }
        }
    }
}


//命令解析
//解析一个带FLOAT的QString,调试的时候txt文档最后一个值后面有乱码
void CmdHandle::AnalyseLastWord(QString &str)
{
    int iLen = str.length();
    QChar ch[30];
    memset(ch,0,30);
    QChar chTest;
    int j=0;
    for (int i=0;i<iLen||i<29;i++)
    {
        chTest = str.at(i);
        if ((chTest<='9' && chTest>='0') || chTest=='.')
        {
            ch[j]=chTest;
            j++;
        }
        else
            break;
    }
    ch[j] ='\n';
    str.append(ch,30);
}

void CmdHandle::WriteCmd2Para(QString strCmd ,DIGIT_CMD *pDigitCmd)
{
    unsigned char bDot = FALSE;
    char ch;
    float fDistaceVal;
    QString strTemp;
    unsigned char bHasDirSymbol = TRUE;
    int iLength = strCmd.length();
    int iIndex = FindFirstLabel(strCmd);
    int iMpIndex = 0;
    AxisDigitPara stMoveUser;
    unsigned char bDirMove = FALSE ;

    if (iIndex == -1)
        return;
    else
        strCmd = strCmd.right(iLength-iIndex);

    while(strCmd != "")
    {
        strCmd.trimmed();
        iLength = strCmd.length();

        ch= strCmd.at(0).unicode();
        memset(&stMoveUser,0,sizeof(AxisDigitPara));
        stMoveUser.iLabel = GetLableIndex(ch);//获得轴序号

        bHasDirSymbol = TRUE;
        ch = strCmd.at(1).unicode();
        if ('+'==ch || '-'==ch)
        {
            strCmd = strCmd.right(iLength-2);
            if('+' == ch)
                bDirMove = TRUE;
            else
                bDirMove = FALSE;
        }
        else
        {
            bDirMove = TRUE;
            strCmd = strCmd.right(iLength-1);
            bHasDirSymbol = FALSE;
        }

        iIndex = FindFirstLabel(strCmd);//是否有下一个轴存在命令当中
        if (iIndex == -1)
        {
            bDot = HasDot(strCmd);
            strTemp = strCmd;
            strCmd ="";
        }
        else
        {
            strTemp = strCmd.left(iIndex);
            bDot = HasDot(strTemp);
            if (bHasDirSymbol)
                strCmd =strCmd.right(iLength-iIndex-2);
            else
                strCmd =strCmd.right(iLength-iIndex-1);
        }

        strTemp.trimmed();
        QStringtoFloat(strTemp,fDistaceVal);
        if (bDot)
            stMoveUser.iDistance = (short)(fDistaceVal*1000.0);
        else
            stMoveUser.iDistance = (short)fDistaceVal;

        if (!bDirMove)
            stMoveUser.iDistance = 0- stMoveUser.iDistance;

        pDigitCmd->stAxisDigit[iMpIndex].iLabel = stMoveUser.iLabel;
        pDigitCmd->stAxisDigit[iMpIndex].iDistance = stMoveUser.iDistance;
        if(++iMpIndex >= 15)
            break;
    }
    pDigitCmd->iAxisCnt = iMpIndex;
}

int CmdHandle::FindFirstLabel(QString strCmd)
{
    int iIndex =0;
    int i=0;
    char ch;
    int iLength = strCmd.length();
    for (i = 0;i<iLength;i++)
    {
        ch = strCmd.at(i).unicode();
        iIndex = GetLableIndex(ch);
        if(iIndex<MAC_LABEL_COUNT && iIndex>=0)
            return i;
    }
    return -1;
}


//从合法字符串中查找轴的标志，并将轴数字化,返回数字8表示没有查到任何轴
//这里的合法字符串只包含带有轴标志的数字字符串，例如“-x987.124”
int CmdHandle::GetLableIndex(char cComLabel)
{
    char ch[MAC_LABEL_COUNT] = ARR_EDM_LABEL_CHAR;
    int i = 0;
    for (i=0;i<MAC_LABEL_COUNT;i++)
    {
        if(cComLabel ==ch[i])
            break;
    }
    return i;
}


//QString里也许含有非法字符，只提取合法的部分
//输入：str:待转换的字符串；fFloat：转换后的值
void CmdHandle::QStringtoFloat(QString str,float &fFloat)
{
    QString strTemp;
    unsigned char bLaw;
    int iIndex = str.indexOf(".");
    int iIntPart,iFloatPart,iLen;

    fFloat = 0;
    if (iIndex != -1)
    {
        strTemp = str.left(iIndex);
        CheckCmdFig(strTemp,&bLaw);
        if (!bLaw)
            return;
        iIntPart = strTemp.toInt();
        iLen = str.length()-iIndex-1;
        str.right(iLen);//获取小数点后的数字
        if (iLen>=4)
            str =str.left(3);
        iIndex = CheckCmdFig(strTemp,&bLaw);
        if (!bLaw)
        {
            strTemp = strTemp.left(iIndex);
        }
        iFloatPart = str.toInt();

        switch(iLen)
        {
        case 1:
            fFloat = iIntPart + iFloatPart/10.000 + 0.00005;
            break;
        case 2:
            fFloat = iIntPart + iFloatPart/100.000 + 0.00005;
            break;
        case 3:
            fFloat = iIntPart + iFloatPart/1000.000 + 0.00005;
            break;
        default:
            fFloat = iIntPart + iFloatPart/1000.000 + 0.00005;
            break;
        }

    }
    else
    {
        iIndex = CheckCmdFig(str,&bLaw);
        if (!bLaw)
        {
            str = str.left(iIndex);
        }

        iIntPart = str.toInt();
        fFloat = iIntPart/1.000 + 0.00005;
    }

}

//检查数字书否非法
int CmdHandle::CheckCmdFig(QString strCmd,unsigned char * pLaw)
{
    int iLen = strCmd.length();
    int i=0;
    QChar ch;
    for (;i<iLen;i++)
    {
        ch = strCmd.at(i);
        if (ch<'0' || ch >'9')
        {
            *pLaw =FALSE;
            return i;
        }
    }
    return i;
}


void CmdHandle::AnalyseVectCommand(QString strCommand,DIGIT_CMD *pDigitCmd,DIGIT_CMD* pCmdDefault)
{
    int iIndex(-1);
    QString strCmdWay("");
    QString strCmdTemp("");
    DIGIT_CMD_VECT stVectCmd;

    memset(pDigitCmd,0,sizeof(DIGIT_CMD));
    pDigitCmd->iFreq = pCmdDefault->iFreq;
    pDigitCmd->enAim = pCmdDefault->enAim;
    pDigitCmd->enOrbit = pCmdDefault->enOrbit;
    pDigitCmd->enCoor = pCmdDefault->enCoor;

    strCommand = strCommand.trimmed().toUpper();
    memset(&stVectCmd,0,sizeof(DIGIT_CMD_VECT));

    iIndex = FindAndCalSpeed(strCommand);
    if (iIndex!= -1)
        strCommand = strCommand.left(iIndex);

    AnalyseCmdMode(strCommand,pDigitCmd);
    WriteVectCmd2Para(strCommand ,&stVectCmd);
    VectSort(&stVectCmd);
    Vect2DigitCmd(&stVectCmd,pDigitCmd);
}


void CmdHandle::VectSort(DIGIT_CMD_VECT* pVectCmd)
{
    int i=0;
    DIGIT_CMD_VECT stVect;
    memset(&stVect,0,sizeof(DIGIT_CMD_VECT));

    for (int j=0;j<6;j++)
    {
        for (i=0;i<pVectCmd->iAxisCnt;i++)
        {
            if (pVectCmd->stAxisDigit[i].iLabel==j)
            {
                stVect.stAxisDigit[stVect.iAxisCnt].iLabel = j;
                if (j>2)
                    stVect.stAxisDigit[stVect.iAxisCnt].iDistance = 0-pVectCmd->stAxisDigit[i].iDistance;
                else
                    stVect.stAxisDigit[stVect.iAxisCnt].iDistance = pVectCmd->stAxisDigit[i].iDistance;

                stVect.iAxisCnt++;
                break;
            }
        }
        if (pVectCmd->iAxisCnt==i)
        {
            stVect.stAxisDigit[stVect.iAxisCnt].iLabel = j;
            stVect.stAxisDigit[stVect.iAxisCnt].iDistance = 0;
            stVect.iAxisCnt++;
        }
    }
    memcpy(pVectCmd,&stVect,sizeof(DIGIT_CMD_VECT));

}


void CmdHandle::Vect2DigitCmd(DIGIT_CMD_VECT* pVectCmd,DIGIT_CMD* pDigitCmd)
{
    EDM* pEdm = EDM::GetEdmInstance();
    int iWorkSet[MAC_LABEL_COUNT]={0};
    QString strLogWrite;
    strLogWrite = pEdm->m_strSysPath + "\\rec.txt";


    int iLabel_C_xMach = pEdm->m_stSysSet.stSetNoneLabel.iLabel_C_xMachine;
    int iLabel_C_yMach = pEdm->m_stSysSet.stSetNoneLabel.iLabel_C_yMachine;
    int iLabel_C_DValue = pEdm->m_stSysSet.stSetNoneLabel.iLabel_C_z_Dvalue;//z process
    int iLabel_A_C_Center =  pEdm->m_stSysSet.stSetNoneLabel.iLabel_A_C_Center;
    int iLabel_A_Distance = pEdm->m_stSysSet.stSetNoneLabel.iLabel_A_Distance;

    iWorkSet[0] = pEdm->m_stSysSet.stSetNoneLabel.iLabelWorkAbs_X;
    iWorkSet[1] = pEdm->m_stSysSet.stSetNoneLabel.iLabelWorkAbs_Y;

    int iLabel_X_2,iLabel_Y_2,iLabel_C_2,iLabel_C,iLabel_B,iLabel_B_1;
    float fTmp,fTmp2;


    fTmp2 = ((float)(pVectCmd->stAxisDigit[3].iDistance))/100000.0;
    fTmp = ((float)(pVectCmd->stAxisDigit[4].iDistance))/100000.0;
    fTmp = fTmp2*fTmp2 + fTmp*fTmp;
    fTmp2 = (float)(pVectCmd->stAxisDigit[5].iDistance)/100000.0;
    fTmp = sqrt(fTmp);
    iLabel_C = GetAntanAngle(FALSE,pVectCmd->stAxisDigit[4].iDistance,pVectCmd->stAxisDigit[3].iDistance);
    iLabel_B_1 = GetAntanAngle(FALSE,fTmp,fTmp2);
    if (pVectCmd->stAxisDigit[3].iDistance<0)
    {
        iLabel_B_1 = 0 -iLabel_B_1;
    }

    iLabel_B = 0-iLabel_B_1;

    iLabel_X_2 = pVectCmd->stAxisDigit[0].iDistance + iWorkSet[0] - iLabel_C_xMach;
    iLabel_Y_2 = pVectCmd->stAxisDigit[1].iDistance + iWorkSet[1] - iLabel_C_yMach;

    iLabel_C_2 = GetAntanAngle(TRUE,iLabel_Y_2,iLabel_X_2);

    int  iLabel_X_3,iLabel_Y_3,iLabel_Z_3,iLabel_C_3;
    iLabel_C_3 = iLabel_C_2 - iLabel_C;
    fTmp2 = (float)(iLabel_X_2)/1000.0;
    fTmp = (float)(iLabel_Y_2)/1000.0;
    fTmp = fTmp2*fTmp2 + fTmp*fTmp;
    fTmp = sqrt(fTmp);
    iLabel_X_3 = fTmp*1000*cos(((float)iLabel_C_3)*EDM_PI/180000.0);
    iLabel_Y_3 = fTmp*1000*sin(((float)iLabel_C_3)*EDM_PI/180000.0);
    iLabel_Z_3 = pVectCmd->stAxisDigit[2].iDistance +iLabel_C_DValue;//z process

    //B旋转
    int iLabel_X_30,iLabel_Z_30;
    iLabel_X_30 = iLabel_A_C_Center + iLabel_X_3;
    iLabel_Z_30 = iLabel_A_Distance + iLabel_Z_3;

    int iLabel_B_3;
    iLabel_B_3 = GetAntanAngle(TRUE,iLabel_Z_30,iLabel_X_30);


    int iLabel_B_4;
    iLabel_B_4 = iLabel_B_1 + iLabel_B_3;
    int iLabel_X_31,iLabel_Y_31,iLabel_Z_31;

    fTmp2 = (float)(iLabel_X_30)/1000.0;
    fTmp = (float)(iLabel_Z_30)/1000.0;
    fTmp = fTmp2*fTmp2 + fTmp*fTmp;
    fTmp = sqrt(fTmp);
    iLabel_X_31 = fTmp*1000*cos(((float)iLabel_B_4)*EDM_PI/180000.0);
    iLabel_Z_31 = fTmp*1000*sin(((float)iLabel_B_4)*EDM_PI/180000.0);
    iLabel_Y_31 = iLabel_Y_3;

    //写入终值
    pDigitCmd->stAxisDigit[pDigitCmd->iAxisCnt].iLabel = 0;
    pDigitCmd->stAxisDigit[pDigitCmd->iAxisCnt].iDistance =  (iLabel_X_31 - iLabel_A_C_Center);
    pDigitCmd->iAxisCnt++;

    pDigitCmd->stAxisDigit[pDigitCmd->iAxisCnt].iLabel = 1;
    pDigitCmd->stAxisDigit[pDigitCmd->iAxisCnt].iDistance =  (iLabel_Y_31);
    pDigitCmd->iAxisCnt++;

    pDigitCmd->stAxisDigit[pDigitCmd->iAxisCnt].iLabel = 7;
    pDigitCmd->stAxisDigit[pDigitCmd->iAxisCnt].iDistance =  0-(iLabel_Z_31 - iLabel_A_Distance);//z process
    pDigitCmd->iAxisCnt++;

    pDigitCmd->stAxisDigit[pDigitCmd->iAxisCnt].iLabel = 3;
    pDigitCmd->stAxisDigit[pDigitCmd->iAxisCnt].iDistance =  iLabel_B;
    pDigitCmd->iAxisCnt++;

    pDigitCmd->stAxisDigit[pDigitCmd->iAxisCnt].iLabel = 2;
    pDigitCmd->stAxisDigit[pDigitCmd->iAxisCnt].iDistance =  iLabel_C;
    pDigitCmd->iAxisCnt++;

}

//写入矢量参数
void CmdHandle::WriteVectCmd2Para(QString strCmd ,DIGIT_CMD_VECT *pVectCmd)
{
    unsigned char bDot = FALSE;
    char ch;
    float fDistaceVal;
    QString strTemp;
    unsigned char bHasDirSymbol = TRUE;
    int iLength = strCmd.length();
    int iIndex = FindFirstLabel(strCmd);
    int iMpIndex = 0;
    AxisDigitPara stMoveUser;
    unsigned char bDirMove = FALSE ;

    if (iIndex == -1)
        return;
    else
        strCmd = strCmd.right(iLength-iIndex);

    while(strCmd != "")
    {
        strCmd.trimmed();
        iLength = strCmd.length();

        ch= strCmd.at(0).unicode();
        memset(&stMoveUser,0,sizeof(AxisDigitPara));
        stMoveUser.iLabel = GetLableIndexVect(ch);//获得轴序号

        bHasDirSymbol = TRUE;
        ch = strCmd.at(1).unicode();
        if ('+'==ch || '-'==ch)
        {
            strCmd = strCmd.right(iLength-2);
            if('+' == ch)
                bDirMove = TRUE;
            else
                bDirMove = FALSE;
        }
        else
        {
            bDirMove = TRUE;
            strCmd = strCmd.right(iLength-1);
            bHasDirSymbol = FALSE;
        }

        iIndex = FindFirstLabelVect(strCmd);//是否有下一个轴存在命令当中
        if (iIndex == -1)
        {
            bDot = HasDot(strCmd);
            strTemp = strCmd;
            strCmd ="";
        }
        else
        {
            strTemp = strCmd.left(iIndex);
            bDot = HasDot(strTemp);
            if (bHasDirSymbol)
                strCmd =strCmd.right(iLength-iIndex-2);
            else
                strCmd =strCmd.right(iLength-iIndex-1);
        }

        strTemp.trimmed();
        fDistaceVal = strTemp.toFloat();
        if (stMoveUser.iLabel<3)
        {
            stMoveUser.iDistance = (short)(fDistaceVal*1000.0);
        }
        else
        {
            stMoveUser.iDistance = (short)(fDistaceVal*100000.0);
        }


        if (!bDirMove)
            stMoveUser.iDistance = 0- stMoveUser.iDistance;

        pVectCmd->stAxisDigit[iMpIndex].iLabel = stMoveUser.iLabel;
        pVectCmd->stAxisDigit[iMpIndex].iDistance = stMoveUser.iDistance;
        if(++iMpIndex >= 6)
            break;
    }
    pVectCmd->iAxisCnt = iMpIndex;
}


int CmdHandle::FindFirstLabelVect(QString strCmd)
{
    int iIndex =0;
    int i=0;
    char ch;
    int iLength = strCmd.length();
    for (i = 0;i<iLength;i++)
    {
        ch = strCmd.at(i).unicode();
        iIndex = GetLableIndexVect(ch);
        if(iIndex<6 && iIndex>=0)
            return i;
    }
    return -1;
}


int  CmdHandle::GetLableIndexVect(char cComLabel)
{
    char ch[6] = {'X','Y','Z','I','J','K'};
    int i = 0;
    for (i=0;i<6;i++)
    {
        if(cComLabel ==ch[i])
            break;
    }
    return i;
}

int CmdHandle::FindAndCalSpeed(QString strCmd)
{
    int iLength = strCmd.length();
    QString strTemp;
    int iIndex= -1;
    QChar ch;
    for (int i=0;i<iLength;i++)
    {
        ch = strCmd.at(i);
        if (ch == 'F')
        {
            iIndex = i;
            break;
        }
    }
    strTemp = strCmd.right(iLength-iIndex-1);
    strTemp.trimmed();
    iLength = GetSpeedFreq(strTemp.toInt());
    return iIndex;
}

int CmdHandle::FindAndCalDepth(QString strCmd,short& dwDepth)
{
    int iLength = strCmd.length();
    QString strTemp;
    int iIndex= -1;
    char ch;
    unsigned char bDot = FALSE;
    float fDistaceVal= 0;
    for (int i=0;i<iLength;i++)
    {
        ch = strCmd.at(i).unicode();
        if (ch == 'D')
        {
            iIndex = i;
            break;
        }
    }
    strTemp = strCmd.right(iLength-iIndex-1);
    strTemp.trimmed();
    bDot = HasDot(strTemp);
    QStringtoFloat(strTemp,fDistaceVal);
    if (bDot)
        dwDepth = (short)(fDistaceVal*1000.0);
    else
        dwDepth = (short)fDistaceVal;

    return iIndex;
}

//从合法字符串中查找小数点
//这里的合法字符串只包含带有轴标志的数字字符串，例如“-x987.124”
unsigned char CmdHandle::HasDot(QString strCmd)
{
    int iLength =strCmd.length();
    char ch;
    int iLabel;
    for (int i=0;i<iLength;i++)
    {
        ch = strCmd.at(i).unicode();
        iLabel = GetLableIndex(ch);
        if (iLabel>=0 && iLabel<MAC_LABEL_COUNT)
            return FALSE;
        if( ch == '.')
            return TRUE;
    }
    return FALSE;
}


int CmdHandle::GetSpeedFreq(int iSpeed)
{
    int iFreq =MAC_INT_FREQ;
    float fSpeedMax = EDM_INT_SPEED_MAX;

    if (iSpeed>0&&iSpeed<=EDM_INT_SPEED_MAX)
    {
        iFreq = ((float)iFreq) * (((float)iSpeed)/fSpeedMax);
    }

    return iFreq;
}

int CmdHandle::GetRAxisFreq(int iSpeed)
{
    int iFreq = 5000;
    if (iSpeed>100)
        iSpeed = 100;
    if (iSpeed>=0 && iSpeed<=100)
    {
        iFreq = iSpeed*100;
    }
    return iFreq;
}

int CmdHandle::GetSpeed(int iFreq)
{
    int iSpeed = EDM_INT_SPEED_MAX;
    float fFreqMax = MAC_INT_FREQ;

    if (iFreq>0 && iFreq<=MAC_INT_FREQ)
    {
        iSpeed =  ((float)iSpeed) * (((float)iFreq)/fFreqMax);
    }

    return iSpeed;
}

void CmdHandle::DigitCmd2QString(DIGIT_CMD *pCmd,QString& strCmd)
{
    QString strAim[3] = {"G90","G91","G92"};
    QString strOribit[4] = {"G00","G01","G02","G03"};
    QString strCoord[6] = {"G54","G55","G56","G57","G58","G59"};
    QChar ch[MAC_LABEL_COUNT] = ARR_EDM_LABEL_CHAR;

    QString strTmp;

    strCmd = "";

    strTmp = strCoord[(int)pCmd->enCoor];
    strTmp += " ";
    strCmd +=strTmp;

    strTmp = strOribit[(int)pCmd->enOrbit];
    strTmp += " ";
    strCmd +=strTmp;

    strTmp = strAim[(int)pCmd->enAim];
    strTmp += " ";
    strCmd +=strTmp;

    for (int i=0;i<pCmd->iAxisCnt;i++)
    {
        strTmp = QString("%1").arg(ch[pCmd->stAxisDigit[i].iLabel]);
        strCmd +=strTmp;
        strTmp = QString("%1").arg(pCmd->stAxisDigit[i].iDistance);
        strCmd +=strTmp;
        strCmd += " ";
    }

    strCmd += " F";
    strTmp = QString("%1").arg(GetSpeed(pCmd->iFreq));
    strCmd +=strTmp;
}


int CmdHandle::GetAntanAngle(unsigned char bFourQuant,float fSon,float fMon)
{
    double dAngle;

    if (fMon==0)
    {
        if (fSon>=0)
            return 90000;
        else
            return -90000;
    }
    else
    {
        dAngle = atan(fSon/fMon);
        dAngle = dAngle*180000.0/EDM_PI;
        if (bFourQuant)
        {
            if (fMon<0)
            {
                return 180000+dAngle;
            }
            else
            {
                return dAngle;
            }
        }
        else
        {
            return dAngle;
        }
    }

    return 0;
}

int CmdHandle::GetALabel_xDis(int iLabel_A,int iRadius,unsigned char bAntiClockWise)
{
    int iDis=0;
    if (!bAntiClockWise)
        iLabel_A = 0-iLabel_A;
    iDis = iRadius*sin(((float)iLabel_A)*EDM_PI/180000.0);;
    return iDis;
}

int CmdHandle::GetALabel_zDis(int iLabel_A,int iRadius,unsigned char bAntiClockWise)
{
    int iDis=0;
    if (!bAntiClockWise)
        iLabel_A = 0-iLabel_A;
    iDis = iRadius - iRadius*cos(((float)iLabel_A)*EDM_PI/180000.0);;
    return iDis;
}

int CmdHandle::GetCLabel_xDis(int iLabel_C,int iRadius,unsigned char bAntiClockWise)
{
    int iDis=0;
    if (!bAntiClockWise)
        iLabel_C = 0-iLabel_C;
    iDis = iRadius*sin(((float)iLabel_C)*EDM_PI/180000.0);;
    return iDis;
}

int CmdHandle::GetCLabel_yDis(int iLabel_C,int iRadius,unsigned char bAntiClockWise)
{
    int iDis=0;
    if (!bAntiClockWise)
        iLabel_C = 0-iLabel_C;
    iDis = iRadius - iRadius*cos(((float)iLabel_C)*EDM_PI/180000.0);;
    return iDis;
}
//from CmdHandle

QString CmdHandle::GetElecPagePara2QString(Elec_Page* pElecPage)
{
    int iCount=0;
    QString str;
    int iPara[20]={0};

    memcpy(iPara,pElecPage,sizeof(Elec_Page));
    iCount =  sizeof(Elec_Page)/sizeof(int);
    str = "";
    for (int i=0;i<iCount;i++)
    {
        str += QString("%1").arg(iPara[i]);
        str +=" ";
    }

    return str;
}

QString CmdHandle::GetElecOralPara2QString(Elec_Oral* pElecOral)
{
    int iCount=0;
    QString str;
    int iPara[20]={0};

    memcpy(iPara,pElecOral,sizeof(Elec_Oral));
    iCount =  sizeof(Elec_Oral)/sizeof(int);
    str = "";
    for (int i=0;i<iCount;i++)
    {
        str += QString("%1").arg(iPara[i]);
        str +=" ";
    }

    return str;
}


QString CmdHandle::GetElecPara2QString(QString strName,MAC_ELEC_PARA* pElecPara)
{
    QString str,strTmp;

    str = strName;
    for (int i=0;i<OP_HOLE_PAGE_MAX;i++)
    {
        strTmp = CmdHandle::GetElecPagePara2QString(&(pElecPara->stElecPage[i]));
        str +=" ";
        str += strTmp;
    }

    str += " ";
    strTmp = CmdHandle::GetElecOralPara2QString(&(pElecPara->stElecOral));
    str += strTmp;

    return str;
}

QString CmdHandle::GetElecPageParaFromQString(QString strCmd,Elec_Page* pElecPage)
{
    int iCount=0;
    QString str,strTmp;
    int iPara[20]={0};
    char ch;
    int iLen;

    strCmd.trimmed();
    strTmp = strCmd;
    iCount = sizeof(Elec_Page)/sizeof(int);
    for (int j=0;j<iCount;j++)
    {
        iLen = strTmp.length();
        for(int i=0;i<iLen;i++)
        {
            ch = strTmp.at(i).unicode();
            if (ch==' ')
            {
                str = strTmp.left(i);
                str.trimmed();
                iPara[j]= str.toInt();
                strTmp = strTmp.right(iLen-i);
                strTmp.trimmed();
                break;
            }
        }
    }
    memcpy(pElecPage,iPara,sizeof(Elec_Page));
    return strTmp;
}

QString CmdHandle::GetElecOralParaFromQString(QString strCmd,Elec_Oral* pElecOral)
{
    int iCount=0;
    QString str,strTmp;
    int iPara[20]={0};
    char ch;
    int iLen;
    int i;

    strCmd.trimmed();
    strTmp = strCmd;
    iCount = sizeof(Elec_Oral)/sizeof(int);
    for (int j=0;j<iCount;j++)
    {
        iLen = strTmp.length();
        for(i=0;i<iLen;i++)
        {
            ch = strTmp.at(i).unicode();
            if (ch==' ')
            {
                str = strTmp.left(i);
                str.trimmed();
                iPara[j]= str.toInt();
                strTmp = strTmp.right(iLen-i);
                strTmp.trimmed();
                break;
            }
        }

        if (i >=iLen)
        {
            iPara[j++]= strTmp.toInt();
            strCmd="";
            break;
        }
    }
    memcpy(pElecOral,iPara,sizeof(Elec_Oral));
    return strTmp;
}


void CmdHandle::GetElecParaFromQString(QString strCmd,QString& strName,MAC_ELEC_PARA* pElecPara)
{
    int iCount=0;
    QString str,strTmp;
    int iLen;
    int i;
    char ch;

    strCmd.trimmed();
    iLen = strCmd.length();
    for(i=0;i<iLen;i++)
    {
        ch = strCmd.at(i).unicode();
        if (ch==' ')
        {
            strName = strCmd.left(i);
            strCmd = strCmd.right(iLen-i);
            break;
        }
    }

    for (int i=0;i<OP_HOLE_PAGE_MAX;i++)
    {
        strCmd.trimmed();
        strCmd = GetElecPageParaFromQString(strCmd,&(pElecPara->stElecPage[i]));
    }
    strCmd.trimmed();
    GetElecOralParaFromQString(strCmd,&(pElecPara->stElecOral));
}


unsigned char CmdHandle::ReadCmdFromFile(QString strPath,QString strFile,vector<QString>* pVector,MAP_ELEC_MAN* pMap)
{
    QString	  strFullName =  strPath + "/" + strFile;
    QFile inFile(strFullName);
    unsigned char bHeadRv = FALSE;
    unsigned char bHeadStart = FALSE;
    QString str;
    QString strName;
    MAC_ELEC_PARA stElec;
    map<QString,MAC_ELEC_PARA>::iterator it=pMap->begin();

    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
              return FALSE;

    pVector->clear();
    while(it!=pMap->end())
        pMap->erase(it++);
    QTextStream in(&inFile);
    while(!in.atEnd())
    {
        str = in.readLine();
        str = str.trimmed().toUpper();
        if (str=="HEADSTART")
		{
			bHeadStart = TRUE;
			continue;
		}		
		if (str=="HEADOVER")
		{
			bHeadRv = TRUE;
			continue;
		}

		if (bHeadStart&& !bHeadRv)
		{
			GetElecParaFromQString(str,strName,&stElec);
			pMap->insert(make_pair(strName,stElec));
		}

		if (bHeadRv)
		{
			if (str!="")
			{
				pVector->push_back(str);
			}			
		}	
    }
    if (!bHeadStart && !bHeadRv)
    {
        EDM* pEdm = EDM::GetEdmInstance();
        str=EDM::m_strElecDefault;
        pEdm->GetElecManElem(str,&stElec);
        pMap->insert(make_pair(str,stElec));
    }


    inFile.close();

    return TRUE;
}

void CmdHandle::LawInt(int& t,int low,int high)
{
    if (low >=high)
        return;

    if (t<low)
        t=low;
    else if (t>high)
        t=high;
}

int CmdHandle::PercentStr2int(QString str)
{
    int iLen = str.length();
    QString strTmp="";
    char ch;
    unsigned char bDot =FALSE;
    unsigned char bPer = FALSE;
    for (int i=0;i<iLen;i++)
    {
        ch = str.at(i).unicode();
        if (ch == '%')
        {
            bPer = TRUE;
            strTmp = str.left(i);
            break;
        }
    }

    for (int i=0;i<iLen;i++)
    {
        ch = str.at(i).unicode();
        if (ch == '.')
        {
            bDot = TRUE;
            break;
        }
    }

    if (bPer)
        return strTmp.toFloat();

    if (bDot)
        return strTmp.toFloat()*100;

    return strTmp.toInt();
}


void CmdHandle::LawOfPara(MAC_ELEC_PARA* pPara)
{
    for (int i=0;i<OP_HOLE_PAGE_MAX;i++)
    {
        LawInt(pPara->stElecPage[i].iOpLen,0,1000000);
        LawInt(pPara->stElecPage[i].iServo,0,100);
        LawInt(pPara->stElecPage[i].iFeedSense,0,100);
        LawInt(pPara->stElecPage[i].iBackSense,0,1000000);
        LawInt(pPara->stElecPage[i].iRotSpeed,1,8);
    }

    LawInt(pPara->stElecOral.iOpHoleIndex,1,pPara->stElecOral.iOpHoleAll);
    if (pPara->stElecOral.iTimeMin<0)
        pPara->stElecOral.iTimeMin = 0;
    if (pPara->stElecOral.iTimeMax<0)
        pPara->stElecOral.iTimeMax = 0;
    if (pPara->stElecOral.iBottomSleep<0)
        pPara->stElecOral.iBottomSleep = 0;
    if (pPara->stElecOral.iRepeatCount<0)
        pPara->stElecOral.iRepeatCount = 0;
    if (pPara->stElecOral.iRepeatLen<0)
        pPara->stElecOral.iRepeatLen = 0;
    LawInt(pPara->stElecOral.iMillServo,1,1000);

    pPara->stElecOral.iOpLenAll = 0;
    for (int i=0;i<OP_HOLE_PAGE_MAX;i++)
    {
        pPara->stElecOral.iOpLenAll += pPara->stElecPage[i].iOpLen;
    }
}
