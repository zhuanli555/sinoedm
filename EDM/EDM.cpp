//edm.cpp edm实现文件


#include "EDM.h"
#include <QDebug>
//机床宏
#define ACTIVE_ADDR_SPACE_NEEDS_INIT 0xFF
#define ISA_INT_FREQ_BASE 100                 //定时器频率计数

#define INTERFACE_HANDWHEEL_IN_FIRST 0x1D0    //手轮所在接口，轴及速率,0x310
#define INTERFACE_HANDWHEEL_IN_SECOND 0x1D2   //手轮所在接口，脉冲,0x312

EDM* EDM::m_pEdm = NULL;
QString EDM::m_strElecDefault="DEFAULT";

EDM::EDM(QObject *parent):QObject(parent)
{
	memset(&m_stEdmComm,0,sizeof(MAC_COMMON));
	memset(&m_stEdmKpInt,0,sizeof(MAC_KPINT));
	memset(&m_stEdmInterfaceOut,0,sizeof(MAC_INTERFACE_OUT));
	memset(&m_stEdmInterfaceIn,0,sizeof(MAC_INTERFACE_IN));
	memset(&m_stStatus,0,sizeof(Mac_Status));
    memset(&m_stEdmOpEntile,0,sizeof(MAC_HANDLE_ENTILE));
    memset(&m_stAxisAdjust,0,sizeof(Axis_Adjust));
    m_pEdmAdoSys = new EDM_Db();
	m_bMachFault = FALSE;
    m_strMakeUpFile = "";
}


EDM* EDM::GetEdmInstance()
{
	if (!m_pEdm)
		m_pEdm = new EDM();
	return m_pEdm;
}

void EDM::DelEdm()
{
	if (m_pEdm)
	{
		delete m_pEdm;
		m_pEdm = NULL;
	}
}


EDM::~EDM()
{
	if (m_pEdmAdoSys)
	{
		delete m_pEdmAdoSys;
		m_pEdmAdoSys = NULL;
    }
}

unsigned long EDM::EdmInit()
{
	short dwStatus;

    m_stEdmInterfaceOut.btO140 = 0xFF;
    m_stEdmInterfaceOut.btO144 = 0xFF;
	m_stEdmInterfaceOut.btO184 = 0xFF;
    m_stEdmInterfaceOut.btO188 = 0xFB;
	m_stEdmInterfaceOut.btO18C = 0x07;//中断号设置
    m_stEdmInterfaceOut.btO190 = 0xFB;
    m_stEdmInterfaceOut.btO198 = 0xFF;//控制电流档位
    m_stEdmInterfaceOut.btO199 = 0xFF;
    m_stEdmInterfaceOut.btO1C0 = 0xFF;
    m_stEdmInterfaceOut.btO1C4 = 0xFF;

    EdmReadMacPara();
	fd = ::open("/dev/short",O_RDWR);
    if(fd < 0)
    {
		EdmClose();
		qDebug()<<"open /dev/short failed";
        return 0;
    }
    ::write(fd,&m_stEdmInterfaceOut,sizeof(MAC_INTERFACE_OUT));
    dwStatus = ioctl(fd,IOC_COMM_INIT,&m_stEdmComm);
	if (dwStatus ==1)
    {
        dwStatus = ioctl(fd,IOC_KP_INIT,&m_stEdmKpInt);
		if (dwStatus ==1)
		{
			return 1;
		}
    }
	return 0;
}

void EDM::GetMacPara(MAC_SYSTEM_SET* pSysSet)
{
	memcpy(pSysSet,&m_stSysSet,sizeof(MAC_SYSTEM_SET));
}

unsigned long EDM::EdmClose()
{
    ::close(fd);
	return 1;
}

bool EDM::GetEdmComm()
{
    short dwStatus;
    dwStatus = ioctl(fd,IOC_COMM_TO_USER,&m_stEdmComm);
	return dwStatus==1;
}

bool EDM::GetEdmMacPassPara(MAC_PASS_PARA* pPass)
{
	short dwStatus;

    memset(pPass,0,sizeof(MAC_PASS_PARA));
    dwStatus = ioctl(fd,IOC_PASS_CTL,pPass);
	return dwStatus==1;
}

int EDM::GetEdmAxisWorkPos(int iLabel)
{
	int iRet = m_stEdmComm.stMoveCtrlComm[iLabel].iMachPos - m_stEdmComm.stMoveCtrlComm[iLabel].iWorkPosSet;
	if (m_stEdmKpInt.stAxisCtrlKp[iLabel].stSoftPara.bRotateLabel)
	{
		iRet %= 360000;
		if (iRet<0)
		   iRet +=360000;
	}
	return iRet;
}

//获取端口数据
bool EDM::GetEdmStatusData()
{
	short dwStatus;
	int iLabel=0;
    GetEdmComm();
	memcpy(&m_stEdmShowData.stComm,&m_stEdmComm,sizeof(MAC_COMMON));
	for (iLabel=0;iLabel<MAC_LABEL_COUNT;iLabel++)
		m_stEdmShowData.iWorkPos[iLabel] = GetEdmAxisWorkPos(iLabel);
	m_stEdmShowData.enCoorType = (EDM_COOR_TYPE)m_iWorkIndex;
	memcpy(&m_stEdmShowData.stEdmInterfaceOut,&m_stEdmInterfaceOut,sizeof(MAC_INTERFACE_OUT));

    dwStatus = ioctl(fd,IOC_INTERFACE_IN,&m_stEdmInterfaceIn);
	if (dwStatus==1)
	{
		m_bMachFault = FALSE;
		memcpy(&m_stEdmShowData.stEdmInterfaceIn,&m_stEdmInterfaceIn,sizeof(MAC_INTERFACE_IN));
		m_stStatus.bDirect = CalcDirectBool(&m_stEdmShowData.stEdmInterfaceIn);
		for (iLabel=0;iLabel<MAC_LABEL_COUNT;iLabel++)
		{
			if (!m_stEdmKpInt.stAxisCtrlKp[iLabel].stSoftPara.bUse)
			{
				m_stEdmShowData.stHardCtl.stHardCtlUser[iLabel].bPosLimit = FALSE;
				m_stEdmShowData.stHardCtl.stHardCtlUser[iLabel].bNegLimit = FALSE;
				m_stEdmShowData.stHardCtl.stHardCtlUser[iLabel].bAlarm = FALSE;
			}
			else
			{
				if (m_stSysSet.bSetLimitNoUse[iLabel] && m_stEdmComm.enMvStatus!=RULE_RTZERO)
				{
					m_stEdmShowData.stHardCtl.stHardCtlUser[iLabel].bPosLimit = FALSE;
					m_stEdmShowData.stHardCtl.stHardCtlUser[iLabel].bNegLimit = FALSE;
				}
				else
				{
					m_stEdmShowData.stHardCtl.stHardCtlUser[iLabel].bPosLimit = CalcLimitBool(&m_stEdmShowData.stEdmInterfaceIn,iLabel,TRUE,
						m_stEdmKpInt.stAxisCtrlKp[iLabel].stSoftPara.bDirectMotor);
					m_stEdmShowData.stHardCtl.stHardCtlUser[iLabel].bNegLimit = CalcLimitBool(&m_stEdmShowData.stEdmInterfaceIn,iLabel,FALSE,
						m_stEdmKpInt.stAxisCtrlKp[iLabel].stSoftPara.bDirectMotor);
				}
				
				m_stEdmShowData.stHardCtl.stHardCtlUser[iLabel].bAlarm = CalcAlarmBool(&m_stEdmShowData.stEdmInterfaceIn,iLabel);

				m_bMachFault = m_bMachFault||m_stEdmShowData.stHardCtl.stHardCtlUser[iLabel].bPosLimit 
					           || m_stEdmShowData.stHardCtl.stHardCtlUser[iLabel].bNegLimit
							   || m_stEdmShowData.stHardCtl.stHardCtlUser[iLabel].bAlarm;
			}
		}

        m_bMachFault = m_bMachFault || m_stStatus.bDirect;
		m_stStatus.bLubricate = m_stEdmShowData.stEdmInterfaceIn.btI144 & 0x01;
		m_stStatus.bAxisOffset = GetAxisOffset();
		memcpy(&m_stEdmShowData.stStatus,&m_stStatus,sizeof(Mac_Status));
		EdmRedLump(m_bMachFault);
		return true;
	}	

	return false;
}

int EDM::GetSpeed(int iFreq)
{
	int iSpeed = EDM_INT_SPEED_MAX;
	float fFreqMax = MAC_INT_FREQ;

	if (iFreq>0 && iFreq<=MAC_INT_FREQ)
	{
		iSpeed =  ((float)iSpeed) * (((float)iFreq)/fFreqMax);
	}

	return iSpeed;
}

bool EDM::EdmSendMovePara(DIGIT_CMD* pMacUser)
{
	short dwStatus;
	bool bSwitchOver = false;
    QString str;	

    if (m_iWorkIndex != (int)pMacUser->enCoor)
    {
        if (SwitchWorkIndex((int)pMacUser->enCoor))
        {
            bSwitchOver = true;
        }
        else
            return false;
    }

    if (bSwitchOver)
    {
        for (int i=0;i<50000;i++)
        {
        }
    }
    dwStatus = ioctl(fd,IOC_MOVEPARA_FROM_USER,pMacUser);
    if (dwStatus==1)
    {
        CmdHandle::DigitCmd2QString(pMacUser,str);
        qDebug()<<str;
        return true;
    }

	return false;
}

void EDM::EdmStop()
{
	CloseHardWare();
    memset(&m_stAxisAdjust,0,sizeof(Axis_Adjust));
	EdmStopMove(TRUE);
}

bool EDM::EdmStopMove(unsigned long bStatus)
{
	short dwStatus;
    QString str = "stop move";
    m_stEdmOpEntile.bStop = TRUE;
    dwStatus = ioctl(fd,IOC_MAC_OPERATE,&m_stEdmOpEntile);
    m_stEdmOpEntile.bStop = FALSE;
	if (dwStatus ==1)
	{
		 m_stStatus.bStop = bStatus;

		return true;
    }
	return false;
}


void EDM::EdmStopSignClose()
{
	m_stStatus.bStop = FALSE;
}

bool EDM::EdmRtZero(int iLabel)
{
	short dwStatus;

	if (m_stEdmComm.enMvStatus==RULE_RTZERO)
		return false;

	m_stEdmOpEntile.bRtZero = TRUE;
    m_stEdmOpEntile.iLabel = iLabel;
    dwStatus = ioctl(fd,IOC_MAC_OPERATE,&m_stEdmOpEntile);
	if (dwStatus == 1)
		return true;

	EdmZeroSignClose();
	return false;
}

void EDM::EdmZeroSignClose()
{
	if (m_stEdmComm.enMvStatus != RULE_RTZERO)
	{
		m_stEdmOpEntile.bRtZero = FALSE;
		m_stEdmOpEntile.iLabel = -1;
	}	
}

bool EDM::EdmSetProtect(unsigned long bProtect)
{
    short dwStatus;
	if (m_stEdmComm.enMvStatus==RULE_RTZERO)
        return false;
    m_stEdmOpEntile.bNoProtect = !bProtect;
    dwStatus = ioctl(fd,IOC_MAC_OPERATE,&m_stEdmOpEntile);
	if (dwStatus == 1)
	{
		m_stStatus.bNoProtect = m_stEdmOpEntile.bNoProtect;		
		return true;
	}	

	m_stEdmOpEntile.bNoProtect = bProtect;
	return false;
}

int EDM::EdmHandProcess()
{
	return  HandBoxProcess();
}

//输入：iLabel：轴标志号 0:X轴；1:Y轴；2:C轴；3:W轴；4:A轴；5:B轴；
int EDM::HandBoxProcess()
{
	DIGIT_CMD stDigitCmd;
	static int iSpeedFreq[3]={MAC_INT_FREQ,2000,166};
    static unsigned char btOut[3] = {0xFB,0xFD,0xFE};
    static int iLabel_xyz[4] ={0,1,6};
    static int iLabel_wc[3] ={3,2};
    static int iLabel_ab[2] = {4,5};
	static int iIndex = 0;
	static int iAdd[3] = {2000,387,5};
	static int iSpeedSum =0;
	static bool bStop= false;
	static bool bHas = false;
	unsigned char btComp = 0x01;
	int dwStatus=0;
	unsigned char btTmp;
	unsigned char btTmp1;
    unsigned char btTmp2;
    unsigned char btTmp3;
	bool bDirMove = false;

	stDigitCmd.iAxisCnt=0;
    btTmp1 = m_stEdmInterfaceIn.btI148&0x3C;
    btTmp2 = m_stEdmInterfaceIn.btI144&0x3F;
    btTmp3 = m_stEdmInterfaceIn.btI1C4&0x0F;

    if (btTmp2<63 ||  btTmp1<60 || btTmp3<15)
	{
		bHas = true;
		bStop= true;
		memset(&stDigitCmd,0,sizeof(DIGIT_CMD));
		stDigitCmd.enAim = AIM_G91;
		stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
		stDigitCmd.iFreq = iSpeedFreq[iIndex];
		stDigitCmd.iAxisCnt = 1;

        if (btTmp2<63)//xyz
		{			 
            for (;dwStatus<6;dwStatus++)
			{
				btTmp = btTmp2&btComp;
				if (btTmp == 0)
				{
					break;
				}
				btComp =btComp<<1;
			}
			if (dwStatus%2==0)
				bDirMove = true;
            stDigitCmd.stAxisDigit[0].iLabel = iLabel_xyz[dwStatus/2];
		}
        else if(btTmp1<60)//wc
		{
            btComp = 0x04;
            for (dwStatus = 0;dwStatus<4;dwStatus++)
			{
                btTmp = btTmp1&btComp;
				if (btTmp == 0)
				{
					break;
				}
				btComp =btComp<<1;
			}
			if (dwStatus%2==0)
				bDirMove = true;
            stDigitCmd.stAxisDigit[0].iLabel = iLabel_wc[dwStatus/2];
		}
        else {
            btComp = 0x01;
            for (dwStatus = 0;dwStatus<4;dwStatus++)
            {
                btTmp = btTmp3&btComp;
                if (btTmp == 0)
                {
                    break;
                }
                btComp =btComp<<1;
            }
            if (dwStatus%2==0)
                bDirMove = true;
            stDigitCmd.stAxisDigit[0].iLabel = iLabel_ab[dwStatus/2];
        }
	}
	else
		bHas = false;

	if (bStop && !bHas)
	{
		EdmStopMove(FALSE);
		bStop = FALSE;			
	}
//获取速度
    btTmp = m_stEdmInterfaceIn.btI148&0x40;
    if (m_stEdmInterfaceIn.btI148 && btTmp==0 && ++iSpeedSum>6)
	{
		iSpeedSum = 0;
		if (++iIndex>=3)
			iIndex=0;
        m_stEdmInterfaceOut.btO144 |=0x07;
        m_stEdmInterfaceOut.btO144 &=btOut[iIndex];
		::write(fd,&m_stEdmInterfaceOut,sizeof(MAC_INTERFACE_OUT));
	}

	if (stDigitCmd.iAxisCnt==1)
	{
		stDigitCmd.stAxisDigit[0].iDistance = bDirMove?iAdd[iIndex]:0-iAdd[iIndex];
		stDigitCmd.iFreq = iSpeedFreq[iIndex];
        stDigitCmd.bNoCheck = TRUE;
        dwStatus = ioctl(fd,IOC_MOVEPARA_FROM_USER,&stDigitCmd);
	}
	return iSpeedFreq[iIndex];
}


//硬件的操作
void EDM::CloseHardWare()
{
	//硬件的关闭
    m_stStatus.bPumpLow = 0;
    m_stStatus.bShake = 0;
    m_stStatus.bPrune= 0;
    m_stStatus.bPower = 0;
    m_stEdmInterfaceOut.btO188 |=0xFF;
	::write(fd,&m_stEdmInterfaceOut,sizeof(MAC_INTERFACE_OUT));
}


bool EDM::EdmLowPump(unsigned long bOpen)
{
    QString str;
	if (bOpen)
	{
		m_stEdmInterfaceOut.btO188 &=0xBF;
		str = "open low pump";
	}
	else
	{
		m_stEdmInterfaceOut.btO188 |=0x40;
		str = "close low pump";
	}

	::write(fd,&m_stEdmInterfaceOut,sizeof(MAC_INTERFACE_OUT));
	m_stStatus.bPumpLow = bOpen;

	return true;
}

bool EDM::EdmSetShake(unsigned long bShake)
{
    QString str;
    if (bShake)
    {
        m_stEdmInterfaceOut.btO188 &=0xF7;
        str = "open shake";
    }
    else
    {
        m_stEdmInterfaceOut.btO188 |=0x08;
        str = "close shake";
    }

    ::write(fd,&m_stEdmInterfaceOut,sizeof(MAC_INTERFACE_OUT));
    m_stStatus.bShake = bShake;

    return true;
}

//电源
bool EDM::EdmPower(unsigned long bOpen)
{
	if (bOpen)
        m_stEdmInterfaceOut.btO188 &=0xFD;
	else
        m_stEdmInterfaceOut.btO188 |=0x02;

	::write(fd,&m_stEdmInterfaceOut,sizeof(MAC_INTERFACE_OUT));
	m_stStatus.bPower = bOpen;

	return true;
}

//修电级 true是正常加工 false是反修
bool EDM::EdmPrune(unsigned long bOpen)
{
	if (bOpen)
        m_stEdmInterfaceOut.btO188 &=0xFE;
	else
        m_stEdmInterfaceOut.btO188 |=0x01;

	::write(fd,&m_stEdmInterfaceOut,sizeof(MAC_INTERFACE_OUT));
	m_stStatus.bPrune = bOpen;
	return true;
}

//beep告警
bool EDM::EdmHummer(unsigned long bOpen)
{
	if (bOpen)
        m_stEdmInterfaceOut.btO188 &=0x7F;
	else
        m_stEdmInterfaceOut.btO188 |=0x80;

	::write(fd,&m_stEdmInterfaceOut,sizeof(MAC_INTERFACE_OUT));

	return true;
}

bool EDM::EdmRedLump(unsigned long bRed)
{
	static bool bStatictRed = false;

	if (bStatictRed != bRed)
	{
		bStatictRed = bRed;
		if (bRed)
			m_stEdmInterfaceOut.btO1C4 &=0xFE;
		else
			m_stEdmInterfaceOut.btO1C4 |=0x01;
		::write(fd,&m_stEdmInterfaceOut,sizeof(MAC_INTERFACE_OUT));
	}
	
	return true;
}

bool EDM::EdmYellowLump(unsigned long bYellow)
{
	static bool bStatictYellow = false;
	if (bStatictYellow != bYellow)
	{
		bStatictYellow = bYellow;
		if (bYellow)
			m_stEdmInterfaceOut.btO1C4 &=0xFD;
		else
			m_stEdmInterfaceOut.btO1C4 |=0x02;

		::write(fd,&m_stEdmInterfaceOut,sizeof(MAC_INTERFACE_OUT));
	}
	                                          
	return true;
}

void EDM::EdmReadMacPara()
{
	m_pEdmAdoSys->GetWorkPosSetIndex(m_iWorkIndex);
	m_pEdmAdoSys->GetEdmCommPara(&m_stEdmComm,m_iWorkIndex);
	m_pEdmAdoSys->GetAllCoor(m_iCoor);
	m_pEdmAdoSys->GetEdmKpIntPara(&m_stEdmKpInt,&m_stSysSet);
    m_pEdmAdoSys->GetPrunePara(&m_stMacOther);
	m_pEdmAdoSys->GetOpName(m_strOpName);
    m_pEdmAdoSys->GetElecMan(&mp_ElecMan);
	m_pEdmAdoSys->GetMacSystemPara(&m_stEdmKpInt,&m_stSysSet);
	for (int i=0;i<MAC_LABEL_COUNT;i++)
	{
        m_pEdmAdoSys->GetAxisLuoBuData(m_stEdmKpInt.stAxisCtrlKp[i].stSoftPara.cSymbol,
			m_stEdmKpInt.stAxisCtrlKp[i].stLuoBuVal.iLuobuPoint,
			MAC_LABEL_MOST_LONG);
    }
}

void EDM::EdmSaveMacComm()
{
	for (int i=0;i<MAC_LABEL_COUNT;i++)
	{
		m_iCoor[i][m_iWorkIndex] = m_stEdmComm.stMoveCtrlComm[i].iWorkPosSet;
	}
	m_pEdmAdoSys->SaveEdmCommPara(&m_stEdmShowData.stComm,m_iWorkIndex,m_iCoor);
}

//设置电参数
int EDM::WriteElecPara(Elec_Page *pElecPara,QString strFunc)
{
    unsigned char btTmp;
    static Elec_Page elec;
    bool bWrite = false;
    //设置脉宽
    if(elec.iTon != pElecPara->iTon)
    {
        elec.iTon = pElecPara->iTon;
        btTmp = GetElecTonVal(pElecPara->iTon);
        btTmp = btTmp<<4;
        m_stEdmInterfaceOut.btO184 |= 0xF0;
        m_stEdmInterfaceOut.btO184 &= btTmp;
        bWrite = true;
    }
    //设置脉停
    if(elec.iToff != pElecPara->iToff)
    {
        elec.iToff = pElecPara->iToff;
        btTmp = GetElecToffVal(pElecPara->iToff);
        m_stEdmInterfaceOut.btO184 |= 0x0F;
        m_stEdmInterfaceOut.btO184 &= btTmp;
        bWrite = true;
    }
    //低压电流
    if(elec.iElecLow != pElecPara->iElecLow)
    {
        elec.iElecLow = pElecPara->iElecLow;
        btTmp = GetElecCurLowVal(pElecPara->iElecLow);
        m_stEdmInterfaceOut.btO198 |= 0xFF;
        m_stEdmInterfaceOut.btO198 &= btTmp;
        if(pElecPara->iElecLow == 9)
        {
            m_stEdmInterfaceOut.btO188 |= 0x02;
            m_stEdmInterfaceOut.btO188 &= 0xFE;
        }else if(pElecPara->iElecLow == 10)
        {
            m_stEdmInterfaceOut.btO188 |= 0x01;
            m_stEdmInterfaceOut.btO188 &= 0xFD;
        }else if(pElecPara->iElecLow == 11)
        {
            m_stEdmInterfaceOut.btO188 &= 0xFC;
        }else{
            m_stEdmInterfaceOut.btO188 |= 0x03;
        }
        bWrite = true;
    }

    //高压电流(加工电流)
    if(elec.iElecHigh != pElecPara->iElecHigh)
    {
        elec.iElecHigh = pElecPara->iElecHigh;
        btTmp = GetElecCurHighVal(pElecPara->iElecHigh);
        m_stEdmInterfaceOut.btO199 |= 0xFF;
        m_stEdmInterfaceOut.btO199 &= btTmp;
        bWrite = true;
    }
    //电容
    if(elec.iCap != pElecPara->iCap)
    {
        elec.iCap = pElecPara->iCap;
        btTmp = (64-pElecPara->iCap)&0xFF;
        m_stEdmInterfaceOut.btO190 |= 0x08;
        if(btTmp&0x20)
        {
            m_stEdmInterfaceOut.btO190 &= 0xFF;
        }else{
            m_stEdmInterfaceOut.btO190 &= 0xF7;
        }
        m_stEdmInterfaceOut.btO144 |= 0xF8;
        m_stEdmInterfaceOut.btO144 &= ((btTmp&0x1F)<<3|0x07);
        bWrite = true;
    }
    //伺服给定
    if(elec.iServo != pElecPara->iServo)
    {
        elec.iServo = pElecPara->iServo;
        SetServoToGive(elec.iServo);
        bWrite = true;
    }
    //设定振动
    if(elec.iShake != pElecPara->iShake||elec.iShakeSense != pElecPara->iShakeSense)
    {
        elec.iShake = pElecPara->iShake;
        elec.iShakeSense = pElecPara->iShakeSense;
        SetShakePara(elec.iShake,elec.iShakeSense);
        bWrite = true;
    }
    if(bWrite)::write(fd,&m_stEdmInterfaceOut,sizeof(MAC_INTERFACE_OUT));
	return 0;
}


//设定伺服
void EDM::SetServoToGive(int iPercent)
{
    int iConverge =  (iPercent - 30) * 4;
    unsigned char btServo;

    if (iConverge>=10 && iConverge<=240)
        btServo = iConverge;
    else
    {
        if (iConverge < 10)
            btServo = 10;
        if (iConverge > 240)
            btServo = 240;
    }

    ioctl(fd,IOC_SERVO,&btServo);
}

//设定振动
void EDM::SetShakePara(int shake,int shakeSense)
{
    int ishake,ishakeSense;
    int i,j;
    Shake_S shakePara;
    memset(&shakePara,0,sizeof(Shake_S));
    if (shake < 300)
        ishake = 300;
    if (shake > 800)
        ishake = 800;
    if (shakeSense < 5)
        ishakeSense = 5;
    if (shakeSense > 95)
        ishakeSense = 95;
    i = 1000000/(ishake*256);
    j = i*shakeSense/100;
    shakePara.iShake = i;
    shakePara.iShakeSense = j;
    ioctl(fd,IOC_SHAKE,&shakePara);
}

bool EDM::GetAxisOffset()
{
	static int iOffsetDelay=0;
	bool bAxisOffset = false;
	int iOffset = 0;

	if (m_stEdmComm.enMvStatus==RULE_MOVE_OVER)
	{
		if (iOffsetDelay<10000)
			iOffsetDelay++;
	}
	else
    {
        iOffsetDelay = 0;
	}

	for (int i=0;i<MAC_LABEL_COUNT;i++)
	{
		
		if (m_stSysSet.bPosMonitor[i])
		{
			if (iOffsetDelay>10)
			{
				iOffset = m_stSysSet.iPosPrecision[i];
			}
			else
			{
				iOffset = m_stSysSet.iPosPrecision[i] * 10;
			}

            if (abs(m_stEdmComm.stMoveCtrlComm[i].iMachPos-m_stEdmComm.stMoveCtrlComm[i].iRasilPos) > iOffset)
			{
				bAxisOffset = true;
			}
		}
	}
	return bAxisOffset;
}

unsigned char EDM::FindElecManElem(QString str)
{
    QString strTmp;
    map<QString,MAC_ELEC_PARA>::iterator it;
    it=mp_ElecMan.begin();
    str = str.toUpper();
    while(it!=mp_ElecMan.end())
    {
        strTmp = it->first;
        strTmp = strTmp.toUpper();

        if (strTmp==str)
        {
            return TRUE;
        }
        ++it;
    }
    return FALSE;
}


void EDM::GetElecManElem(QString str,MAC_ELEC_PARA* pElecMan)
{
    if (FindElecManElem(str))
    {
        memcpy(pElecMan,&mp_ElecMan[str],sizeof(MAC_ELEC_PARA));
    }
}

void EDM::SaveElecElem(QString str,MAC_ELEC_PARA* pElec)
{
    MAC_ELEC_PARA stElec;
    memcpy(&stElec,pElec,sizeof(MAC_ELEC_PARA));
    m_pEdmAdoSys->SaveElecMan(str,&stElec);
}

bool EDM::SaveMacPara(MAC_SYSTEM_SET* pSysSet)
{
	MAC_SYSTEM_SET sys;
	short dwStatus;

    memcpy(&sys,pSysSet,sizeof(MAC_SYSTEM_SET));
	dwStatus = ioctl(fd,IOC_SYSTEM_SET,&sys);
	if (dwStatus ==1)
	{
		memcpy(&m_stSysSet,pSysSet,sizeof(MAC_SYSTEM_SET));
		m_pEdmAdoSys->SaveMacSystemPara(&m_stSysSet);
		m_pEdmAdoSys->SaveEdmKpIntPara(&m_stEdmKpInt,&m_stSysSet);
		return true;
	}	
	return false;	
}

bool EDM::SwitchWorkIndex(int iSwitch)
{
	DIGIT_CMD cmd;
	int iIndex = -1;
	short dwStatus;
	if (m_stEdmComm.enMvStatus == RULE_MOVE_OVER)
	{
		if (iSwitch==m_iWorkIndex)
		{
			return true;
		}
		EdmSaveMacComm();
		iIndex = m_iWorkIndex;
		if (iSwitch>=0 && iSwitch<6)
		{
			m_iWorkIndex = iSwitch;
		}
		else
			return false;

		memset(&cmd,0,sizeof(DIGIT_CMD));
		cmd.enAim = AIM_G92;
		cmd.enOrbit = ORBIT_G01;
		cmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
		cmd.iFreq = MAC_INT_FREQ;
		for (int i=0;i<MAC_LABEL_COUNT;i++)
		{
			cmd.stAxisDigit[i].iLabel = i;
			cmd.stAxisDigit[i].iDistance = m_stEdmComm.stMoveCtrlComm[i].iMachPos-m_iCoor[i][m_iWorkIndex];
			cmd.iAxisCnt++;
        }
        dwStatus = ioctl(fd,IOC_MOVEPARA_FROM_USER,&cmd);
		if (dwStatus==1)
		{
			m_pEdmAdoSys->SaveWorkPosSetIndex(m_iWorkIndex);			
			return true;
		}
		else
		{
			m_iWorkIndex = iIndex;
		}
	}

	return false;
}

void EDM::ReSetWorkPosSetByIndex(int iIndex,int iWork[][6])
{
	if (iIndex>=0 && iIndex<6)
	{
		for (int i=0;i<MAC_LABEL_COUNT;i++)
		{
			m_iCoor[i][iIndex] = iWork[i][iIndex];		
		}
		
	}
	m_pEdmAdoSys->SaveEdmWorkSet(iIndex,m_iCoor);
}


void EDM::SetAxisAdjust(int iLabel, unsigned char bDir)
{
	memset(&m_stAxisAdjust,0,sizeof(Axis_Adjust));
	m_stAxisAdjust.bAdjust = TRUE;
	m_stAxisAdjust.iLabel = iLabel;
	m_stAxisAdjust.bDir = bDir;
}
//对刀
void EDM::EdmAxisAdjust()
{
	int iAim = 500000;
	DIGIT_CMD stDigitCmd;
	int iADJUST = 3;

	if (m_stAxisAdjust.bAdjust)
	{
		if (m_stEdmComm.enMvStatus == RULE_MOVE_OVER)
		{
			if (m_stAxisAdjust.bOver)
			{
				EdmSetProtect(TRUE);
				memset(&m_stAxisAdjust,0,sizeof(Axis_Adjust));
				return;
			}

			if (m_stAxisAdjust.iAdjustCnt>=iADJUST)
			{
				EdmSetProtect(FALSE);

				memset(&stDigitCmd,0,sizeof(DIGIT_CMD));
				stDigitCmd.iFreq = 2500;
				stDigitCmd.enAim = AIM_G90;
				stDigitCmd.enOrbit = ORBIT_G00;
				stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
				stDigitCmd.stAxisDigit[0].iDistance = m_stAxisAdjust.iPos/iADJUST 
					- m_stEdmComm.stMoveCtrlComm[m_stAxisAdjust.iLabel].iWorkPosSet;
				stDigitCmd.stAxisDigit[0].iLabel = m_stAxisAdjust.iLabel;
				stDigitCmd.iAxisCnt = 1;

				if (EdmSendMovePara(&stDigitCmd))
				{
					m_stAxisAdjust.bOver = TRUE;
				}
				return;

			}

			if (m_stAxisAdjust.iStage ==0)
			{
				if (EdmSetProtect(FALSE))
				{
					if (m_stAxisAdjust.bDir)
						iAim = 500;
					else
						iAim = -500;

					memset(&stDigitCmd,0,sizeof(DIGIT_CMD));
					stDigitCmd.iFreq = 1000;
					stDigitCmd.enAim = AIM_G91;
					stDigitCmd.enOrbit = ORBIT_G00;
					stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
					stDigitCmd.stAxisDigit[0].iDistance = iAim;
					stDigitCmd.stAxisDigit[0].iLabel = m_stAxisAdjust.iLabel;
					stDigitCmd.iAxisCnt = 1;

					if (EdmSendMovePara(&stDigitCmd))
					{
						m_stAxisAdjust.iStage++;
					}
				}
			}
			else if (m_stAxisAdjust.iStage==1)
			{
				if (EdmSetProtect(TRUE))
				{
					if (m_stAxisAdjust.bDir == FALSE)
						iAim = -500000;

					memset(&stDigitCmd,0,sizeof(DIGIT_CMD));
					stDigitCmd.iFreq = 2500;
					stDigitCmd.enAim = AIM_G91;
					stDigitCmd.enOrbit = ORBIT_G00;
					stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
					stDigitCmd.stAxisDigit[0].iDistance = iAim;
					stDigitCmd.stAxisDigit[0].iLabel = m_stAxisAdjust.iLabel;
					stDigitCmd.iAxisCnt = 1;

					if (EdmSendMovePara(&stDigitCmd))
					{
						m_stAxisAdjust.iStage++;
					}
				}				
			}
			else if (m_stAxisAdjust.iStage ==2)
			{
				if (m_stStatus.bDirect)
				{
					if (EdmSetProtect(FALSE))
					{
						while (!EdmStopMove(FALSE))
						{
						}
						if (m_stAxisAdjust.bDir)
							iAim = -10;
						else
							iAim = 10;

						memset(&stDigitCmd,0,sizeof(DIGIT_CMD));
						stDigitCmd.iFreq = 1000;
						stDigitCmd.enAim = AIM_G91;
						stDigitCmd.enOrbit = ORBIT_G00;
						stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
						stDigitCmd.stAxisDigit[0].iDistance = iAim;
						stDigitCmd.stAxisDigit[0].iLabel = m_stAxisAdjust.iLabel;
						stDigitCmd.iAxisCnt = 1;

						if (EdmSendMovePara(&stDigitCmd))
						{
							m_stAxisAdjust.iStage = 2;
						}
					}
				}
				else
				{
					m_stAxisAdjust.iStage++;
				}
			}
			else if (m_stAxisAdjust.iStage==3)
			{
				if (!m_stStatus.bDirect)
				{
					if (EdmSetProtect(FALSE))
					{
						while (!EdmStopMove(FALSE))
						{
						}						
						GetEdmComm();
						m_stAxisAdjust.iPos += m_stEdmComm.stMoveCtrlComm[m_stAxisAdjust.iLabel].iMachPos;

						if (m_stAxisAdjust.bDir)
							iAim = -1000;
						else
							iAim = 1000;

						memset(&stDigitCmd,0,sizeof(DIGIT_CMD));
						stDigitCmd.iFreq = 10000;
						stDigitCmd.enAim = AIM_G91;						
						stDigitCmd.enOrbit = ORBIT_G00;
						stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
						stDigitCmd.stAxisDigit[0].iDistance = iAim;
						stDigitCmd.stAxisDigit[0].iLabel = m_stAxisAdjust.iLabel;
						stDigitCmd.iAxisCnt = 1;

						if (EdmSendMovePara(&stDigitCmd))
						{
							m_stAxisAdjust.iAdjustCnt++;
							m_stAxisAdjust.iStage = 0;
						}

					}
				}
				else
					m_stAxisAdjust.iStage = 2;
			}
		}		
	}
}

void EDM::SetValMakeUp(int iVal_C,int iVal_X)
{
	m_mpMakeUp_C.insert(make_pair(iVal_C,iVal_X));
}

void EDM::SetMakeUpFile(QString strFile)
{	
	if (strFile!=m_strMakeUpFile)
	{
		ClearMakeUpVal();
		m_strMakeUpFile = strFile;
	}
}

void EDM::ClearMakeUpVal()
{
	map<int,int>::iterator it=m_mpMakeUp_C.begin();
	while (it != m_mpMakeUp_C.end())
	{
		m_mpMakeUp_C.erase(it++);
	}
}

bool EDM::HasMakeUpVal()
{
	map<int,int>::iterator it=m_mpMakeUp_C.begin();
	if (it != m_mpMakeUp_C.end())
	{
		return true;
	}

	return false;
}

bool EDM::GetMakeUpVal(int iVal_C,int* pVal)
{
	bool bExist_1 = false;
	int iVal_1,iVal_2;

	map<int,int>::iterator it=m_mpMakeUp_C.begin();
	if (it != m_mpMakeUp_C.end())
	{
		iVal_1 = it->second;
	}

	while (it != m_mpMakeUp_C.end())
	{
		if (it->first==iVal_C)
		{
			bExist_1 = true;
			iVal_2 = it->second;
		}
		it++;
	}

	if (bExist_1)
	{
		*pVal = iVal_2 - iVal_1;
		return true;
	}

	return false;
}


void EDM::AutoClearMakeUpVal(DIGIT_CMD* pMacUser)
{
	int iLabel =0;
	if (pMacUser->enAim == AIM_G92)
	{		
		for (int i=0;i<pMacUser->iAxisCnt;i++)
		{
			iLabel = pMacUser->stAxisDigit[i].iLabel;
			if (iLabel==0||iLabel==1||iLabel==2)
			{
				ClearMakeUpVal();
			}
		}
	}
}

