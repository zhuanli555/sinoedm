#include "cmdhandle.h"
#include <math.h>

#define  EDM_PI 3.1415926


CmdHandle::CmdHandle(unsigned char bVectCmd,QString strCommand,DIGIT_CMD *pDigitCmd,DIGIT_CMD* pCmdDefault)
{
    if (!bVectCmd)
    {
        AnalyseCommand(strCommand,pDigitCmd,pCmdDefault);
    }
}

CmdHandle::~CmdHandle()
{

}

void CmdHandle::AnalyseCommand(QString strCommand,DIGIT_CMD *pDigitCmd,DIGIT_CMD* pCmdDefault)
{
    int iIndex(-1);

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
        strCmd = strCmd.trimmed();
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

        strTemp = strTemp.trimmed();
        QStringtoFloat(strTemp,fDistaceVal);
        if (bDot)
            stMoveUser.iDistance = (long)(fDistaceVal*1000.0);
        else
            stMoveUser.iDistance = (long)fDistaceVal;

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
    bool bLaw = true;
    int iIndex = str.indexOf(".");
    int iIntPart,iFloatPart,iLen;
    fFloat = 0;
    if (iIndex != -1)
    {
        strTemp = str.left(iIndex);
        CheckCmdFig(strTemp,&bLaw);
        if (bLaw == false)
            return;
        iIntPart = strTemp.toInt();
        iLen = str.length()-iIndex-1;
        strTemp = str.right(iLen);//获取小数点后的数字
        if(strTemp == ".")strTemp="0";
        if(iLen>=4)strTemp =strTemp.left(3);
        iFloatPart = strTemp.toInt();
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
int CmdHandle::CheckCmdFig(QString strCmd,bool* pLaw)
{
    int iLen = strCmd.length();
    int i=0;
    char ch;
    for (;i<iLen;i++)
    {
        ch = strCmd.at(i).unicode();
        if (ch<'0' || ch >'9')
        {
            *pLaw =false;
            return i;
        }
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
    int iPara[20]={0};
    int i = 0;

    QString strTmp = strCmd.trimmed();
    QStringList list = strTmp.split(' ');

    foreach (const QString &str, list) {
        iPara[i++] = str.trimmed().toInt();
    }
    memcpy(pElecPage,iPara,sizeof(Elec_Page));
    return strTmp;
}

QString CmdHandle::GetElecOralParaFromQString(QString strCmd,Elec_Oral* pElecOral)
{
    int iPara[20]={0};
    int i = 0;

    QString strTmp = strCmd.trimmed();
    QStringList list = strTmp.split(' ');
    foreach (const QString &str, list) {
        iPara[i++] = str.trimmed().toInt();
    }

    memcpy(pElecOral,iPara,sizeof(Elec_Oral));
    return strTmp;
}


unsigned char CmdHandle::ReadCmdFromFile(QString strPath,QString strFile,vector<QString>& pVector,MAP_ELEC_MAN* pMap)
{
    QString	  strFullName =  strPath + "/" + strFile;
    QFile inFile(strFullName);
    QString str;
    MAC_ELEC_PARA stElec;
    map<QString,MAC_ELEC_PARA>::iterator it=pMap->begin();

    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
              return 0;

    pVector.clear();
    while(it!=pMap->end())
        pMap->erase(it++);
    QTextStream in(&inFile);
    while(!in.atEnd())
    {
        str = in.readLine();
        if (!str.isEmpty())
        {
            str = str.trimmed().toUpper();
            pVector.push_back(str);
        }
    }
    EDM* pEdm = EDM::GetEdmInstance();
    str=EDM::m_strElecDefault;
    if(!pEdm->FindElecManElem(strFile))
    {
        pEdm->GetElecManElem(str,&stElec);
    }else{
        pEdm->GetElecManElem(strFile,&stElec);
    }

    pMap->insert(make_pair(strFile,stElec));
    inFile.close();

    return 1;
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
        LawInt(pPara->stElecPage[i].iShake,300,800);
        LawInt(pPara->stElecPage[i].iShakeSense,0,100);
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
