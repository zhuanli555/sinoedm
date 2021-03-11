//edm.cpp edm实现文件


#include "EDM.h"
#include <QDebug>
//机床宏
#define ACTIVE_ADDR_SPACE_NEEDS_INIT 0xFF
#define ISA_INT_FREQ_BASE 100                 //定时器频率计数

#define INTERFACE_HANDWHEEL_IN_FIRST 0x1D0    //手轮所在接口，轴及速率,0x310
#define INTERFACE_HANDWHEEL_IN_SECOND 0x1D2   //手轮所在接口，脉冲,0x312

EDM* EDM::m_pEdm = NULL;
QString EDM::m_strElecDefault="P0";

EDM::EDM(QObject *parent):QObject(parent)
{
	memset(&m_stEdmComm,0,sizeof(MAC_COMMON));
	memset(&m_stEdmKpInt,0,sizeof(MAC_KPINT));
	memset(&m_stEdmInterfaceOut,0,sizeof(MAC_INTERFACE_OUT));
	memset(&m_stEdmInterfaceIn,0,sizeof(MAC_INTERFACE_IN));
	memset(&m_stStatus,0,sizeof(Mac_Status));
	memset(&m_stEdmOpEntile,0,sizeof(MAC_HANDLE_ENTILE));
	memset(&m_stAdjustCircle,0,sizeof(ADJUST_CIRCLE));
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
    m_stEdmInterfaceOut.btO144 = 0xFD;
	m_stEdmInterfaceOut.btO184 = 0xFF;
	m_stEdmInterfaceOut.btO188 = 0xFF;
	m_stEdmInterfaceOut.btO18C = 0x07;//中断号设置
	m_stEdmInterfaceOut.btO190 = 0xFB;
    m_stEdmInterfaceOut.btO198 = 0x7F;//控制电流档位
	m_stEdmInterfaceOut.btO199 = 0xBF;
    m_stEdmInterfaceOut.btO1C0 = 0xFF;
    m_stEdmInterfaceOut.btO1C4 = 0xFF;

	fd = ::open("/dev/short",O_RDWR);
    if(fd < 0)
    {
		EdmClose();
		qDebug()<<"open /dev/short failed";
        return 0;
    }
    ::write(fd,&m_stEdmInterfaceOut,sizeof(MAC_INTERFACE_OUT));
    EdmReadMacPara();
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
		m_stStatus.bRalarm = m_stEdmShowData.stEdmInterfaceIn.btI148 & 0x02;
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

void EDM::EdmSetVectCenter_CPos()
{
	m_stSysSet.stSetNoneLabel.iLabel_C_xMachine =  m_stEdmComm.stMoveCtrlComm[0].iMachPos;
	m_stSysSet.stSetNoneLabel.iLabel_C_yMachine =  m_stEdmComm.stMoveCtrlComm[1].iMachPos;
	SaveMacPara(&m_stSysSet);
	RetCenter_C_G59();
}

void EDM::RetCenter_C_G59()
{
	DIGIT_CMD cmd;
	m_iCoor[0][5] = m_stSysSet.stSetNoneLabel.iLabel_C_xMachine;
	m_iCoor[1][5] =  m_stSysSet.stSetNoneLabel.iLabel_C_yMachine;
	m_pEdmAdoSys->SaveEdmCommPara(&m_stEdmShowData.stComm,5,m_iCoor);
	if ((EDM_COOR_TYPE)m_iWorkIndex == COOR_G59)
	{
		memset(&cmd,0,sizeof(DIGIT_CMD));
		cmd.enAim = AIM_G92;
		cmd.enCoor = COOR_G59;
		cmd.enOrbit = ORBIT_G01;
		cmd.iFreq = MAC_INT_FREQ;
		cmd.stAxisDigit[cmd.iAxisCnt].iLabel =0;
		cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_stEdmComm.stMoveCtrlComm[0].iMachPos - m_stSysSet.stSetNoneLabel.iLabel_C_xMachine;
		cmd.iAxisCnt++;
		cmd.stAxisDigit[cmd.iAxisCnt].iLabel =1;
		cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_stEdmComm.stMoveCtrlComm[1].iMachPos - m_stSysSet.stSetNoneLabel.iLabel_C_yMachine;
		cmd.iAxisCnt++;
		while (!EdmSendMovePara(&cmd))
		{
			for (int i=0;i<1000;i++)
			{
			}
		}
	}
}

void EDM::EdmSetVectWorkPos()
{
	m_stSysSet.stSetNoneLabel.iLabelWorkAbs_X =  m_stEdmComm.stMoveCtrlComm[0].iMachPos;
	m_stSysSet.stSetNoneLabel.iLabelWorkAbs_Y =  m_stEdmComm.stMoveCtrlComm[1].iMachPos;
	SaveMacPara(&m_stSysSet);
}

void EDM::EdmStop()
{
	CloseHardWare();
	memset(&m_stAxisAdjust,0,sizeof(Axis_Adjust));
	memset(&m_stAdjustCircle,0,sizeof(ADJUST_CIRCLE));
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


//输入：iLabel：轴标志号 0:X轴；1:Y轴；2:C轴；3:A轴；4:S轴；5:W轴；6:B轴；7:Z轴；
int EDM::HandBoxProcess()
{
	DIGIT_CMD stDigitCmd;
	static int iSpeedFreq[3]={MAC_INT_FREQ,2000,166};
	static unsigned char btOut[3] = {0x1B,0x1D,0x1E};
	static int iLabel_F[4] ={0,1,7,4};
	static int iLabel_S[3] ={3,5,2};
	static int iIndex = 0;
	static int iAdd[3] = {2000,387,5};
	static int iSpeedSum =0;
	static bool bStop= false;
	static bool bHas = false;
	unsigned char btComp = 0x01;
	short dwStatus=0;
	unsigned char btTmp;
	unsigned char btTmp1;
    unsigned char btTmp2;
	bool bDirMove = false;

	stDigitCmd.iAxisCnt=0;
	btTmp1 = m_stEdmInterfaceIn.btI1C8&0x3F;
	btTmp2 = m_stEdmInterfaceIn.btI1C4;

	if (btTmp2<255 ||  btTmp1<63)
	{
		bHas = true;
		bStop= true;
		memset(&stDigitCmd,0,sizeof(DIGIT_CMD));
		stDigitCmd.enAim = AIM_G91;
		stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
		stDigitCmd.iFreq = iSpeedFreq[iIndex];
		stDigitCmd.iAxisCnt = 1;

		if (btTmp2<255)
		{			 
			for (;dwStatus<8;dwStatus++)
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
			stDigitCmd.stAxisDigit[0].iLabel = iLabel_F[dwStatus/2];
		}
		else
		{
			for (;dwStatus<6;dwStatus++)
			{
				btTmp = m_stEdmInterfaceIn.btI1C8&btComp;
				if (btTmp == 0)
				{
					break;
				}
				btComp =btComp<<1;
			}
			if (dwStatus%2==0)
				bDirMove = true;
			stDigitCmd.stAxisDigit[0].iLabel = iLabel_S[dwStatus/2];
		}
	}
	else
		bHas = false;

	if (bStop && !bHas)
	{
		EdmStopMove(FALSE);
		bStop = FALSE;			
	}

	btTmp = m_stEdmInterfaceIn.btI1C8&0x40;
	if (btTmp==0 && ++iSpeedSum>6)
	{
		iSpeedSum = 0;
		if (++iIndex>=3)
			iIndex=0;
		m_stEdmInterfaceOut.btO1C4 |=0x07;
		m_stEdmInterfaceOut.btO1C4 &=btOut[iIndex];
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
    m_stEdmInterfaceOut.btO188 |=0x3F;
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

int EDM::GetRAxisFreq(int iSpeed)
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
    //m_pEdmAdoSys->GetPrunePara(&m_stMacOther);
	m_pEdmAdoSys->GetOpName(m_strOpName);
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
	unsigned char btCal;
    QString str;
    QString strTmp = "write elec:";
	unsigned char bDebug = TRUE;
	int iForbit;

	//防止乱码
	iForbit = pElecPara->iTon + pElecPara->iElecLow;
	if ( iForbit>170 || iForbit <0)
	{
		return -1;
	}

	//设置脉宽
	btTmp = GetElecTonVal(pElecPara->iTon);
	btCal = 0x0F&btTmp;
	m_stEdmInterfaceOut.btO184 &= 0xF0;
	m_stEdmInterfaceOut.btO184 |= btCal;
	m_stEdmInterfaceOut.btO188 &=0xF7;
	btCal = 0x10&btTmp;
	if (btCal)
		m_stEdmInterfaceOut.btO188 |= 0x08;
	
	//设置脉停
	btTmp = GetElecToffVal(pElecPara->iToff);
	m_stEdmInterfaceOut.btO184 &= 0x0F;
	btTmp = btTmp<<4;
	btTmp &= 0xF0;
	m_stEdmInterfaceOut.btO184 |= btTmp;

	//低压电流
	btTmp = GetElecCurLowVal(pElecPara->iElecLow);
	m_stEdmInterfaceOut.btO190 &= 0xE0;
	m_stEdmInterfaceOut.btO190 =m_stEdmInterfaceOut.btO190 |btTmp;
	
	//高压电流
	btTmp = GetElecCurHighVal(pElecPara->iElecHigh);
	m_stEdmInterfaceOut.btO18C &= 0xE7;
	btTmp = btTmp<<3;	
	m_stEdmInterfaceOut.btO18C |= btTmp;
	
	//电容
	btTmp = GetElecCapVal(pElecPara->iCap);
	m_stEdmInterfaceOut.btO144 = btTmp;

	//旋转
	btTmp = pElecPara->iRotSpeed;
	btTmp &=0x07;
	btTmp = ~btTmp;
	btTmp = btTmp<<5;
	m_stEdmInterfaceOut.btO144 &= 0x1F;
	m_stEdmInterfaceOut.btO144 |= btTmp;

	::write(fd,&m_stEdmInterfaceOut,sizeof(MAC_INTERFACE_OUT));
	//伺服给定
	SetServoToGive(pElecPara->iServo);

    str =  CmdHandle::GetElecPagePara2QString(pElecPara);
	strTmp += str;
	if (bDebug)
	{
		strTmp +=": ";
		strTmp += strFunc;
	}	
	//WriteRecord(strTmp);
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
		if (iOffsetDelay!=0)
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

			if (!(abs(m_stEdmComm.stMoveCtrlComm[i].iMachPos-m_stEdmComm.stMoveCtrlComm[i].iRasilPos) <= iOffset
			|| abs(m_stEdmComm.stMoveCtrlComm[i].iMachPos-m_stEdmComm.stMoveCtrlComm[i].iRasilPos+360000) <= iOffset
			|| abs(m_stEdmComm.stMoveCtrlComm[i].iMachPos-m_stEdmComm.stMoveCtrlComm[i].iRasilPos-360000) <= iOffset))
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

bool EDM::SaveMacPara(MAC_SYSTEM_SET* pSysSet)
{
	MAC_SYSTEM_SET sys;
	short dwStatus;

	memcpy(&sys,pSysSet,sizeof(MAC_SYSTEM_SET));
	RetCenter_C_G59();
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
	int iIndex;
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

bool EDM::GetWorkPosSetByIndex(int iIndex,int iWork[])
{
	if (iIndex>=0 && iIndex<6)
	{
		for (int i=0;i<MAC_LABEL_COUNT;i++)
		{
			iWork[i] = m_iCoor[i][iIndex];
		}

		return true;
	}

	return false;
}

void EDM::ReSetWorkPosSetByIndex(int iIndex,int iWork[])
{
	if (iIndex>=0 && iIndex<6)
	{
		for (int i=0;i<MAC_LABEL_COUNT;i++)
		{
			if (i!=m_stSysSet.stSetNoneLabel.iOpLabel)
			{
				m_iCoor[i][iIndex] = iWork[i];
			}			
		}
		m_pEdmAdoSys->SaveEdmWorkSet(iIndex,m_iCoor);
	}
}


void EDM::SetAxisAdjust(int iLabel, unsigned char bDir)
{
	memset(&m_stAxisAdjust,0,sizeof(Axis_Adjust));
	m_stAxisAdjust.bAdjust = TRUE;
	m_stAxisAdjust.iLabel = iLabel;
	m_stAxisAdjust.bDir = bDir;
}

void EDM::SetAxisAdjustCircleType(ADJUST_CIRCLE* pAdjustCircle)
{
	memcpy(&m_stAdjustCircle,pAdjustCircle,sizeof(ADJUST_CIRCLE));
	m_stAdjustCircle.iVal_X[4] =  m_stEdmComm.stMoveCtrlComm[0].iMachPos;
	m_stAdjustCircle.iVal_Y[4] = m_stEdmComm.stMoveCtrlComm[1].iMachPos;
	m_stAdjustCircle.iIndex = 0;
	m_stAdjustCircle.bAdjust = TRUE;
	m_stAdjustCircle.iIndexCnt = 0;
}

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

void EDM::EdmAxisAdjustCircle()
{
	if (m_stAdjustCircle.bAdjust)
	{
		if (m_stAdjustCircle.enAdjustType == EN_ADJUST_INSIDE)
		{
			EdmAxisAdjustCircleInside();
		}
		else if (m_stAdjustCircle.enAdjustType == EN_ADJUST_OUTSIDE)
		{
			EdmAxisAdjustCircleOutSide();
		}
	}
}

void EDM::EdmAxisAdjustCircleInside()
{
	DIGIT_CMD stDigitCmd;
	int iLabel[4] = {0,0,1,1};
	unsigned char bDir[4] = {TRUE,FALSE,TRUE,FALSE};
	int iVal_Center[2];

	if (!m_stAxisAdjust.bAdjust)
	{
		if (m_stEdmComm.enMvStatus == RULE_MOVE_OVER)
		{
			memset(&stDigitCmd,0,sizeof(DIGIT_CMD));

			if (m_stAdjustCircle.bOver)
			{
				memset(&m_stAdjustCircle,0,sizeof(ADJUST_CIRCLE));
				EdmSetProtect(TRUE);
			}

			if (m_stAdjustCircle.iIndex>3)
			{
				m_stAdjustCircle.iIndex =0;
				m_stAdjustCircle.iIndexCnt++;
			}

			if (m_stAdjustCircle.iIndexCnt==4)
			{
				while (!EdmSetProtect(FALSE))
				{					
				}
				iVal_Center[0] = (m_stAdjustCircle.iVal_X[0] + m_stAdjustCircle.iVal_X[1])/2;
				iVal_Center[1] = (m_stAdjustCircle.iVal_Y[2] + m_stAdjustCircle.iVal_Y[3])/2;
				iVal_Center[0] = iVal_Center[0] - m_stEdmComm.stMoveCtrlComm[0].iWorkPosSet;
				iVal_Center[1] = iVal_Center[1] - m_stEdmComm.stMoveCtrlComm[1].iWorkPosSet;

				stDigitCmd.iFreq = 10000;
				stDigitCmd.enAim = AIM_G90;						
				stDigitCmd.enOrbit = ORBIT_G00;
				stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
				stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iDistance = iVal_Center[0];
				stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iLabel = 0;
				stDigitCmd.iAxisCnt++;

				stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iDistance = iVal_Center[1];
				stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iLabel = 1;
				stDigitCmd.iAxisCnt++;

				if (EdmSendMovePara(&stDigitCmd))
				{
					m_stAdjustCircle.bOver = TRUE;
					return;
				}
			}

			switch(m_stAdjustCircle.iIndex)
			{
			case 0:
				{
					stDigitCmd.iFreq = 10000;
					stDigitCmd.enAim = AIM_G91;						
					stDigitCmd.enOrbit = ORBIT_G00;
					stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
					stDigitCmd.stAxisDigit[0].iDistance = m_stAdjustCircle.iHeight;
					stDigitCmd.stAxisDigit[0].iLabel = m_stSysSet.stSetNoneLabel.iOpLabel;
					stDigitCmd.iAxisCnt = 1;

					if (EdmSendMovePara(&stDigitCmd))
					{
						m_stAdjustCircle.iIndex++;
					}
				}
				break;
			case 1:
				{
					SetAxisAdjust(iLabel[m_stAdjustCircle.iIndexCnt], bDir[m_stAdjustCircle.iIndexCnt]);
					m_stAdjustCircle.iIndex++;
				}
				break;			
			case 2:
				{
					m_stAdjustCircle.iVal_X[m_stAdjustCircle.iIndexCnt] = m_stEdmComm.stMoveCtrlComm[0].iMachPos;
					m_stAdjustCircle.iVal_Y[m_stAdjustCircle.iIndexCnt] = m_stEdmComm.stMoveCtrlComm[1].iMachPos;
					while (!EdmSetProtect(FALSE))
					{						
					}
					stDigitCmd.iFreq = 10000;
					stDigitCmd.enAim = AIM_G90;						
					stDigitCmd.enOrbit = ORBIT_G00;
					stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
					stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iDistance = m_stAdjustCircle.iVal_X[4]
					- m_stEdmComm.stMoveCtrlComm[0].iWorkPosSet;
					stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iLabel = 0;
					stDigitCmd.iAxisCnt++;

					stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iDistance = m_stAdjustCircle.iVal_Y[4]
					- m_stEdmComm.stMoveCtrlComm[1].iWorkPosSet;
					stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iLabel = 1;
					stDigitCmd.iAxisCnt++;

					if (EdmSendMovePara(&stDigitCmd))
					{
						m_stAdjustCircle.iIndex++;
					}
				}
				break;
			case 3:
				{
					stDigitCmd.iFreq = 10000;
					stDigitCmd.enAim = AIM_G91;						
					stDigitCmd.enOrbit = ORBIT_G00;
					stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
					stDigitCmd.stAxisDigit[0].iDistance = 0-m_stAdjustCircle.iHeight;
					stDigitCmd.stAxisDigit[0].iLabel = m_stSysSet.stSetNoneLabel.iOpLabel;
					stDigitCmd.iAxisCnt = 1;

					if (EdmSendMovePara(&stDigitCmd))
					{
						m_stAdjustCircle.iIndex++;
					}
				}
				break;
			default:
				break;
			}
		}
	}
}

void EDM::EdmAxisAdjustCircleOutSide()
{
	DIGIT_CMD stDigitCmd;
	int iLabel[4] = {0,0,1,1};
	unsigned char bDir[4] = {TRUE,FALSE,TRUE,FALSE};
	int iProtect = ((float)m_stAdjustCircle.iSemiDiameter)*0.1;
	int iVal_Center[2];

	iProtect = max(iProtect,2000);	
	if (!m_stAxisAdjust.bAdjust)
	{
		if (m_stEdmComm.enMvStatus == RULE_MOVE_OVER)
		{
			memset(&stDigitCmd,0,sizeof(DIGIT_CMD));

			if (m_stAdjustCircle.bOver)
			{
				memset(&m_stAdjustCircle,0,sizeof(ADJUST_CIRCLE));
				EdmSetProtect(TRUE);
			}

			if (m_stAdjustCircle.iIndex>4)
			{
				m_stAdjustCircle.iIndex =0;
				m_stAdjustCircle.iIndexCnt++;
			}

			if (m_stAdjustCircle.iIndexCnt==4)
			{
				while (!EdmSetProtect(FALSE))
				{					
				}
				iVal_Center[0] = (m_stAdjustCircle.iVal_X[0] + m_stAdjustCircle.iVal_X[1])/2;
				iVal_Center[1] = (m_stAdjustCircle.iVal_Y[2] + m_stAdjustCircle.iVal_Y[3])/2;
				iVal_Center[0] = iVal_Center[0] - m_stEdmComm.stMoveCtrlComm[0].iWorkPosSet;
				iVal_Center[1] = iVal_Center[1] - m_stEdmComm.stMoveCtrlComm[1].iWorkPosSet;

				stDigitCmd.iFreq = 10000;
				stDigitCmd.enAim = AIM_G90;						
				stDigitCmd.enOrbit = ORBIT_G00;
				stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
				stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iDistance = iVal_Center[0];
				stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iLabel = 0;
				stDigitCmd.iAxisCnt++;

				stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iDistance = iVal_Center[1];
				stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iLabel = 1;
				stDigitCmd.iAxisCnt++;

				if (EdmSendMovePara(&stDigitCmd))
				{
					m_stAdjustCircle.bOver = TRUE;
					return;
				}
			}

			switch(m_stAdjustCircle.iIndex)
			{
			case 0:
				{
					stDigitCmd.iFreq = 10000;
					stDigitCmd.enAim = AIM_G91;						
					stDigitCmd.enOrbit = ORBIT_G00;
					stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
					if (bDir[m_stAdjustCircle.iIndexCnt])
						stDigitCmd.stAxisDigit[0].iDistance = 0-m_stAdjustCircle.iSemiDiameter-iProtect;
					else
						stDigitCmd.stAxisDigit[0].iDistance = m_stAdjustCircle.iSemiDiameter+iProtect;

					stDigitCmd.stAxisDigit[0].iLabel = iLabel[m_stAdjustCircle.iIndexCnt];
					stDigitCmd.iAxisCnt = 1;

					if (EdmSendMovePara(&stDigitCmd))
					{
						m_stAdjustCircle.iIndex++;
					}
				}
				break;
			case 1:
				{
					stDigitCmd.iFreq = 10000;
					stDigitCmd.enAim = AIM_G91;						
					stDigitCmd.enOrbit = ORBIT_G00;
					stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
					stDigitCmd.stAxisDigit[0].iDistance = m_stAdjustCircle.iHeight;
					stDigitCmd.stAxisDigit[0].iLabel = m_stSysSet.stSetNoneLabel.iOpLabel;
					stDigitCmd.iAxisCnt = 1;

					if (EdmSendMovePara(&stDigitCmd))
					{
						m_stAdjustCircle.iIndex++;
					}
				}
				break;
			case 2:
				{
					SetAxisAdjust(iLabel[m_stAdjustCircle.iIndexCnt], bDir[m_stAdjustCircle.iIndexCnt]);
					m_stAdjustCircle.iIndex++;
				}
				break;
			case 3:
				{
					stDigitCmd.iFreq = 10000;
					stDigitCmd.enAim = AIM_G91;						
					stDigitCmd.enOrbit = ORBIT_G00;
					stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
					stDigitCmd.stAxisDigit[0].iDistance = 0-m_stAdjustCircle.iHeight;
					stDigitCmd.stAxisDigit[0].iLabel = m_stSysSet.stSetNoneLabel.iOpLabel;
					stDigitCmd.iAxisCnt = 1;

					if (EdmSendMovePara(&stDigitCmd))
					{
						m_stAdjustCircle.iIndex++;
					}
				}
				break;
			case 4:
				{
					m_stAdjustCircle.iVal_X[m_stAdjustCircle.iIndexCnt] = m_stEdmComm.stMoveCtrlComm[0].iMachPos;
					m_stAdjustCircle.iVal_Y[m_stAdjustCircle.iIndexCnt] = m_stEdmComm.stMoveCtrlComm[1].iMachPos;
					while (!EdmSetProtect(FALSE))
					{						
					}
					stDigitCmd.iFreq = 10000;
					stDigitCmd.enAim = AIM_G90;						
					stDigitCmd.enOrbit = ORBIT_G00;
					stDigitCmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;
					stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iDistance = m_stAdjustCircle.iVal_X[4]
					- m_stEdmComm.stMoveCtrlComm[0].iWorkPosSet;
					stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iLabel = 0;
					stDigitCmd.iAxisCnt++;

					stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iDistance = m_stAdjustCircle.iVal_Y[4]
					- m_stEdmComm.stMoveCtrlComm[1].iWorkPosSet;
					stDigitCmd.stAxisDigit[stDigitCmd.iAxisCnt].iLabel = 1;
					stDigitCmd.iAxisCnt++;

					if (EdmSendMovePara(&stDigitCmd))
					{
						m_stAdjustCircle.iIndex++;
					}
				}
				break;
			default:
				break;
			}
		}
	}
}

void EDM::EdmRtCenterC()
{
	DIGIT_CMD  cmd;
	memset(&cmd,0,sizeof(DIGIT_CMD));
	cmd.enAim = AIM_G90;
	cmd.enOrbit = ORBIT_G01;
	cmd.iFreq = 10000;
	cmd.enCoor = (EDM_COOR_TYPE)m_iWorkIndex;

	GetEdmComm();
	if (m_stEdmComm.enMvStatus == RULE_MOVE_OVER)
	{
		cmd.stAxisDigit[cmd.iAxisCnt].iLabel = 0;
		cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_stSysSet.stSetNoneLabel.iLabel_C_xMachine
			-m_stEdmComm.stMoveCtrlComm[0].iWorkPosSet;
		cmd.iAxisCnt++;

		cmd.stAxisDigit[cmd.iAxisCnt].iLabel = 1;
		cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_stSysSet.stSetNoneLabel.iLabel_C_yMachine
			-m_stEdmComm.stMoveCtrlComm[1].iWorkPosSet;
		cmd.iAxisCnt++;

		EdmSendMovePara(&cmd);
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

