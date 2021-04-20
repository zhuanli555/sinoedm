//edm_op_grind.cpp edm_op_grind实现文件
#include "EDM_OP_HOLE.h"
#include <math.h>

#define  EDM_OP_WAIT_COUNT  5
#define  MAX_EDM_BLIND_CYCLE 6
#define  MIN_EDM_BLIND_CYCLE 3
#define  EDM_BLIND_CYCLE_DEVIATE 50

EDM_OP_HOLE::EDM_OP_HOLE(MAC_OPERATE_TYPE enOpType)
{
	memset(&m_stOpCtrl,0,sizeof(HOLE_CTRL));
	m_stOpStatus.enOpType = enOpType;
	m_enOpType = enOpType;
	m_pEdmOpPre = NULL;
    EdmHoleOpTypeInit();
}


EDM_OP_HOLE::~EDM_OP_HOLE()
{
	if (!m_ListStage.empty())
		m_ListStage.clear();

	EdmHoleRecover();
	m_pEdm = NULL;
}

void EDM_OP_HOLE::EdmHoleOpTypeInit()
{
	if (!m_ListStage.empty())
		m_ListStage.clear();

	if (m_stOpStatus.enOpType==OP_HOLE_SING)
	{
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleZeroAdjust_Sing);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHolePrune);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleOpPage);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleRepeat);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleRootSleep);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleRise);		
	} 
	else if (m_stOpStatus.enOpType==OP_HOLE_PROGRAME)
	{
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleRise);
        //m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleUp2Safe);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleSynchro);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleLocation);
        //m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleDownFromSafe);
        m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleZeroAdjust);//电极对零
        m_ListStage.push_back(&EDM_OP_HOLE::EdmHolePrune);//电极修整
        m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleOpPage);//加工
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleRepeat);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleRootSleep);			
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleRise);
	} 
	else if (m_stOpStatus.enOpType==OP_HOLE_MILL)
	{	
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleZeroAdjust);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHolePrune);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleMillPage);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleRise);
	} 
	else if (m_stOpStatus.enOpType==OP_HOLE_SIMULATE)
	{
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleRise);
        //m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleUp2Safe);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleSynchro);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleLocation);
        //m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleDownFromSafe);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleRootSleep);		
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleRise);
	}
	else if (m_stOpStatus.enOpType==OP_HOLE_CHECK_C)
	{
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleSynchro);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleLocation);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmOpMvAheadChkLabel);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmOpMvWaitChkLabel);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmOpMvBackChkLabel);
	}

	m_it = m_ListStage.begin();		
}

void EDM_OP_HOLE::EdmOpSetStart(unsigned char bStart)
{
	MAC_OPERATE_TYPE enOpType;
	enOpType = m_stOpStatus.enOpType;	

	if (bStart)
	{
		if (m_pOpFile->m_enOpFileErr != OP_FILE_NO_ERR 
			|| !m_pOpFile)
		{
			m_stOpStatus.bStart = FALSE;
			m_stOpStatus.stCycle.bPauseCmd = TRUE;
			return;
		}

		EDM_OP::m_bInOp = TRUE;
		EDM_OP::m_bStartCount = TRUE;

		if (m_stOpStatus.bOpOver 
			|| m_stOpStatus.iCmdIndex>= m_pOpFile->m_iCmdNum
			|| m_stOpStatus.iCmdIndex<0)
		{
			memset(&m_stOpCtrl,0,sizeof(HOLE_CTRL));
			memset(&m_stOpStatus,0,sizeof(OP_STATUS));
			m_stOpStatus.enOpType = enOpType;			
			m_stOpCtrl.iWaitCnt = 0;
		}	
		m_stOpStatus.bStart = TRUE;
		SetAllErr();
		m_stOpStatus.stCycle.bPauseCmd = FALSE;
		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		m_stOpCtrl.stZeroCtrl.bMillLast = FALSE;
		m_stOpCtrl.bSynchro = TRUE;

		if (m_stOpStatus.enOpType==OP_HOLE_CHECK_C )
		{
			if (!m_pEdm->HasMakeUpVal())
			{
				m_stOpStatus.iCmdIndex = 0;
			}	
			m_stOpCtrl.bSynchro = FALSE;
        }
	}
	else
	{
		m_stOpStatus.bStart = FALSE;
		EDM_OP::m_bInOp = FALSE; 
		m_stOpStatus.stCycle.bPauseCmd = TRUE;
        while (!m_pEdm->EdmStopMove(FALSE))
		{
		}
        m_pEdm->EdmSetProtect(TRUE);
		SetEdmHolePower(FALSE,FALSE,FALSE);
		m_stOpCtrl.iWaitCnt = 0;
		m_stOpCtrl.stZeroCtrl.bWait = FALSE;
		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
	}
}


void EDM_OP_HOLE::EdmOpCarry()
{
	if (m_stOpStatus.bStart && !m_stOpStatus.bOpOver)
	{
        if (m_stOpStatus.iCmdIndex != m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iOpHoleIndex)
        {
            m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iOpHoleIndex = m_stOpStatus.iCmdIndex;
            m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iOpHoleAll = m_pOpFile->m_iCmdNum;
		}
		if (m_pOpFile)
		{
            m_stOpStatus.errFile = m_pOpFile->m_enOpFileErr;
		}
		
		if (EdmOpErr())
		{
			EdmHoleRecover();
            if (m_stOpStatus.errOp==OP_LEN_POLE)
			{
				EdmOpGoHigh();
			}
			return;
		}		

		if (m_stOpCtrl.iWaitCnt>0)
		{
			m_stOpCtrl.iWaitCnt--;
			return;
		}

		if (ExteedTimeAlarm())
		{
			EdmHoleRecover();
			return;
		}

		EdmHoleCarry();
	}
}

void EDM_OP_HOLE::SetAllErr()
{
    m_stOpStatus.errOp = OP_NO_ERR;
	if (m_pOpFile)
	{
        m_stOpStatus.errFile = m_pOpFile->m_enOpFileErr;
	}
	else
	{
        m_stOpStatus.errFile = OP_FILE_NO_EXIST;
	}
}


void EDM_OP_HOLE::EdmOpOver()
{
	EdmHoleRecover();
	EDM_OP::m_bInOp = FALSE;
	EDM_OP::m_bStartCount = FALSE;
	EDM_OP::m_bSetPower = FALSE;
}

void EDM_OP_HOLE::EdmOpStageRestart()
{
    while (!m_pEdm->EdmStopMove(FALSE))
	{
	}
	m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
}

/**
 * 加工过程
 * 1.遍历m_ListStage执行加工各个步骤
*/
void EDM_OP_HOLE::EdmHoleCarry()
{
    fEdmOpStage pStageCarry;

    CalcDigitCmd();
	if(!m_pEdm->GetEdmComm())
		return;

	if (m_stOpCtrl.stZeroCtrl.bCycleStart)
	{
		if(m_it==m_ListStage.end())
		{
			CycleOver();
		}
		else
        {
			pStageCarry = *m_it;
			if((this->*pStageCarry)())
			{
				m_stOpCtrl.iWaitCnt = 1;
				m_it++;
			}
		}
	}
	else
	{
		if (EdmHoleSynchro())
        {
			m_stOpCtrl.stZeroCtrl.bCycleStart = TRUE;
			m_it = m_ListStage.begin();
            if (m_stOpStatus.enOpType!=OP_HOLE_SING)
			{
				EdmHoleCmdProcess();
			}
		}			
	}

	m_stOpStatus.stCycle.bCycleStart = m_stOpCtrl.stZeroCtrl.bCycleStart;
}


void EDM_OP_HOLE::EdmHoleCmdProcess()
{
	if (m_stOpStatus.iCmdIndex>=m_pOpFile->m_iCmdNum || m_stOpStatus.iCmdIndex<0)
	{
		EdmHoleOverProcess();	
		return;
	}

    m_Cmd = m_pOpFile->m_vCmd[m_stOpStatus.iCmdIndex++];

    if (m_pOpFile->IsPauseCmd(m_Cmd))
		EdmHolePauseProcess();
    else if (m_pOpFile->IsOverCmd(m_Cmd))
        EdmHoleOverProcess();
	else	  
		EdmHoleMvCmdProcess();	
}


void EDM_OP_HOLE::EdmHolePauseProcess()
{
	m_stOpStatus.bStart = FALSE;
	m_stOpStatus.stCycle.bPauseCmd = TRUE;
	m_it=m_ListStage.end();
	SetEdmHolePower(FALSE,FALSE,FALSE);
}


void EDM_OP_HOLE::EdmHoleOverProcess()
{
	if (m_stOpStatus.enOpType==OP_HOLE_CHECK_C)
	{		
		m_stOpStatus.enOpType = m_enOpType;		
		EdmHoleOpTypeInit();
	}
	CycleOver();
	memset(&m_stOpCtrl,0,sizeof(HOLE_CTRL));
	memset(&m_stOpStatus,0,sizeof(OP_STATUS));		
	SetEdmHolePower(FALSE,FALSE,FALSE);
	m_stOpStatus.enOpType = m_enOpType;
	m_stOpStatus.bOpOver = TRUE;
    m_stOpStatus.stCycle.bPauseCmd = TRUE;
    m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iOpHoleIndex = m_pOpFile->m_iCmdNum;
	EdmHoleRecover();
}

void EDM_OP_HOLE::EdmHoleMvCmdProcess()
{
	DIGIT_CMD cmdDefault;
    CmdHandle* pCmdHandle;
	memset(&cmdDefault,0,sizeof(DIGIT_CMD));
    memset(&m_stMvCmd,0,sizeof(DIGIT_CMD));
    pCmdHandle = new CmdHandle(FALSE,m_Cmd,&m_stMvCmd,&cmdDefault);
	m_stMvCmd.iFreq = min(m_stMvCmd.iFreq,m_iWholeFreq);
	m_stMvCmd.stOp.bShortDis = TRUE;
	delete pCmdHandle;
}

//主轴回升
unsigned char EDM_OP_HOLE::EdmHoleRise()
{
	unsigned char bRise = FALSE;
    DIGIT_CMD cmd;
    INFO_PRINT();
	int iPos = m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iMachPos 
		- m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iWorkPosSet;

	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{
        m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		return TRUE;
	}

	m_stOpStatus.stCycle.iCycleIndex = 1;

	if (!m_stOpCtrl.stZeroCtrl.bStageLast)
    {
        if ((m_pEdm->m_stSysSet.stSetNoneLabel.bOpDir && iPos>m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iRisePos)
            || (!m_pEdm->m_stSysSet.stSetNoneLabel.bOpDir && iPos<m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iRisePos))
		{
			bRise = TRUE;
		}


		if (bRise)
		{
            memset(&cmd,0,sizeof(DIGIT_CMD));
            cmd.enAim = AIM_G90;
            cmd.enOrbit = ORBIT_G00;
            cmd.enCoor = m_pOpFile->m_enCoor;
            cmd.iAxisCnt = 1;
            cmd.stAxisDigit[0].iDistance = m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iRisePos;
            cmd.stAxisDigit[0].iLabel = m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel;

            if (m_pEdm->EdmSendMovePara(&cmd))
            {
                m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
                return FALSE;
            }
		}
		else
		{
			return TRUE;
		}
	}

	return FALSE;
}

unsigned char EDM_OP_HOLE::EdmHoleUp2Safe()
{
    DIGIT_CMD cmd;
    INFO_PRINT();

	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;

	if (m_pEdm->m_stSysSet.iAxistLabel<0 || m_pEdm->m_stSysSet.iAxistLabel>=MAC_LABEL_COUNT)
	{
		return TRUE;
	}

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{
		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;

		return TRUE;
	}

    if (!m_stOpCtrl.stZeroCtrl.bStageLast && m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel==6)
	{	
        memset(&cmd,0,sizeof(DIGIT_CMD));
        cmd.enAim = AIM_G90;
        cmd.enCoor = m_pOpFile->m_enCoor;
        cmd.enOrbit = ORBIT_G01;
        cmd.iFreq = MAC_INT_FREQ;
        cmd.stAxisDigit[cmd.iAxisCnt].iLabel = m_pEdm->m_stSysSet.iAxistLabel;
        cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iSafePos;
        cmd.iAxisCnt++;
        if (m_pEdm->EdmSendMovePara(&cmd))
        {
            m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
        }
	}
	else
	{
        m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
	}
	return FALSE;
}


unsigned char EDM_OP_HOLE::EdmHoleDownFromSafe()
{
	DIGIT_CMD cmd;
    INFO_PRINT();

	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;

	if (m_pEdm->m_stSysSet.iAxistLabel<0 || m_pEdm->m_stSysSet.iAxistLabel>=MAC_LABEL_COUNT)
	{
		if (m_stOpCtrl.stZeroCtrl.bEmptyMove)
        {
			CycleOver();
		}
		return TRUE;
	}

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{
        m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		if (m_stOpCtrl.stZeroCtrl.bEmptyMove)
		{
            INFO_PRINT();
			CycleOver();
		}		
		return TRUE;
	}

    if (!m_stOpCtrl.stZeroCtrl.bStageLast && m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel==6)
    {

        memset(&cmd,0,sizeof(DIGIT_CMD));
        cmd.enAim = AIM_G90;
        cmd.enCoor = m_pOpFile->m_enCoor;
        cmd.enOrbit = ORBIT_G01;
        cmd.iFreq = m_iWholeFreq;
        cmd.stAxisDigit[cmd.iAxisCnt].iLabel = m_pEdm->m_stSysSet.iAxistLabel;
        cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_iSafeLabelMacPos - m_pEdm->m_stEdmComm.stMoveCtrlComm[ m_pEdm->m_stSysSet.iAxistLabel].iWorkPosSet;
        cmd.iAxisCnt++;
        if (m_pEdm->EdmSendMovePara(&cmd))
        {
            m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
        }
    }
	else
	{
        m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
	}
	return FALSE;
}

unsigned char EDM_OP_HOLE::EdmHoleLocation()
{
    DIGIT_CMD cmd2Send;
	unsigned char bEmptyMove=FALSE;
    INFO_PRINT();
	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{
        m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;

		return TRUE;
	}

	m_stOpStatus.stCycle.iCycleIndex = 2;

	if (!m_stOpCtrl.stZeroCtrl.bStageLast)
    {
		m_stOpCtrl.bSynchro = FALSE;
		memcpy(&cmd2Send,&m_stMvCmd,sizeof(DIGIT_CMD));
        if (m_stOpStatus.enOpType!=OP_HOLE_CHECK_C)
        {
            m_stOpCtrl.bSynchro = TRUE;
		}

		if (cmd2Send.enOrbit == ORBIT_G00)
		{
			bEmptyMove = TRUE;
		}

        while (!m_pEdm->EdmSetProtect(TRUE))
		{
		}
        //cmd2Send.enOrbit = ORBIT_G01;
		if(m_pEdm->EdmSendMovePara(&cmd2Send))
		{
			m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
			m_stOpCtrl.stZeroCtrl.bEmptyMove = bEmptyMove;
			return FALSE;
		}
	}	

	return FALSE;	
}

//加工同步
unsigned char EDM_OP_HOLE::EdmHoleSynchro()
{
	int iCmdIndex;
    QString str;

	if (m_stOpStatus.enOpType == OP_HOLE_SING)
	{
		return TRUE;
	}

	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;

	if (!m_stOpCtrl.bSynchro)
	{
		return TRUE;
	}
    m_stOpStatus.stCycle.iCycleIndex = 2;
	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{
        INFO_PRINT();
        m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		return TRUE;
	}
	if (!m_stOpCtrl.stZeroCtrl.bStageLast)
    {
        INFO_PRINT();
		iCmdIndex = m_stOpStatus.iCmdIndex+1;
		while (iCmdIndex<m_pOpFile->m_iCmdNum)
		{
			str = m_pOpFile->m_vCmd[iCmdIndex];
            if (!m_pOpFile->IsPauseCmd(str))
			{
				break;
			}
			iCmdIndex++;
		}
        if (iCmdIndex >= m_pOpFile->m_iCmdNum)
		{
			return TRUE;
        }

        while (!m_pEdm->EdmSetProtect(TRUE))
		{
        }
        m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
        return FALSE;
	}	

	return FALSE;
}

//电极对零
unsigned char EDM_OP_HOLE::EdmHoleZeroAdjust_Sing()
{
	DIGIT_CMD cmd;

	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{
		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		m_stOpCtrl.stZeroCtrl.bWait = FALSE;

		if (PoleLenAlarm())
		{
			SetEdmHolePower(FALSE,FALSE,FALSE);				
			return FALSE;
		}
		return TRUE;
	}

	m_stOpStatus.stCycle.iCycleIndex = 3;
	m_stOpStatus.stCycle.iOpPage = -1;

	if (!m_stOpCtrl.stZeroCtrl.bStageLast)
    {
		if (!m_stOpCtrl.stZeroCtrl.bWait)
		{
			m_stOpCtrl.stZeroCtrl.bWait = TRUE;
			m_stOpCtrl.iWaitCnt = 1;
			if (!EDM_OP::m_bSetPower)
			{
				SetEdmHolePower(TRUE,FALSE,FALSE);
				m_stOpCtrl.iWaitCnt = EDM_OP_WAIT_COUNT;				
			}
			return FALSE;
		}

		memset(&cmd,0,sizeof(DIGIT_CMD));

		cmd.enAim = AIM_G92;
		cmd.enOrbit = ORBIT_G00;
		cmd.enCoor = m_pOpFile->m_enCoor;
		cmd.iFreq = MAC_INT_FREQ;
		cmd.iAxisCnt++;
		cmd.stAxisDigit[0].iLabel = m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel;
		cmd.stAxisDigit[0].iDistance = 0;

		if (m_pEdm->EdmSendMovePara(&cmd))
		{
			m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
			m_stOpCtrl.stZeroCtrl.bWait = FALSE;
		}
	}

	return FALSE;
}

//电极对零
unsigned char EDM_OP_HOLE::EdmHoleZeroAdjust()
{
	Elec_Page stElec;
	DIGIT_CMD cmd;
    INFO_PRINT();
	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{
		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		m_stOpCtrl.stZeroCtrl.bWait = FALSE;

		if (PoleLenAlarm())
		{
			SetEdmHolePower(FALSE,FALSE,FALSE);	
			return FALSE;
		}
		return TRUE;
	}
    INFO_PRINT();
	m_stOpStatus.stCycle.iCycleIndex = 3;
	m_stOpStatus.stCycle.iOpPage = -1;

	if (!m_stOpCtrl.stZeroCtrl.bStageLast)
	{

		if (!m_stOpCtrl.stZeroCtrl.bWait)
		{
			m_stOpCtrl.stZeroCtrl.bWait = TRUE;
			memset(&stElec,0,sizeof(Elec_Page));
            stElec.iTon  = 300;
            stElec.iToff = 300;
            stElec.iElecLow = 3;
			stElec.iElecHigh = 0;
			stElec.iCap = 0;
			stElec.iServo = 75;
            stElec.iShake = 300;
            stElec.iShakeSense = 50;
			m_pEdm->WriteElecPara(&stElec,"EdmHoleZeroAdjust");

			m_stOpCtrl.iWaitCnt = 1;
			if (!EDM_OP::m_bSetPower)
			{
				SetEdmHolePower(TRUE,FALSE,FALSE);
				m_stOpCtrl.iWaitCnt = EDM_OP_WAIT_COUNT;				
			}
			return FALSE;
		}
        INFO_PRINT();
		memset(&cmd,0,sizeof(DIGIT_CMD));
		cmd.stOp.bOpenPower = TRUE;
		cmd.stOp.enOpType = OP_HOLE_DISCHARGE;
		cmd.stOp.iBackSense = 100;
		cmd.stOp.iFeedSense = 100;

		cmd.enAim = AIM_G91;
		cmd.enOrbit = ORBIT_G01;
		cmd.enCoor = m_pOpFile->m_enCoor;
		cmd.iFreq = MAC_INT_FREQ;
		cmd.iAxisCnt++;
		cmd.stAxisDigit[0].iLabel = m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel;
		if (m_pEdm->m_stSysSet.stSetNoneLabel.bOpDir)
			cmd.stAxisDigit[0].iDistance = 1000000;
		else
			cmd.stAxisDigit[0].iDistance = -1000000;

		if (m_pEdm->EdmSendMovePara(&cmd))
		{
			m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
			m_stOpCtrl.stZeroCtrl.bWait = FALSE;
		}
	}
	
	return FALSE;
}

//电极修整
unsigned char EDM_OP_HOLE::EdmHolePrune()
{
	DIGIT_CMD cmd;
    INFO_PRINT();
	
	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
	   return FALSE;	

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
    {
		SetEdmHolePower(TRUE,FALSE,FALSE);
		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		m_stOpCtrl.iWaitCnt = EDM_OP_WAIT_COUNT;
		m_stOpCtrl.stZeroCtrl.bWait = FALSE;
		return TRUE;
	}

	m_stOpStatus.stCycle.iOpPage = 0;
	m_stOpStatus.stCycle.iCycleIndex = 4;
	m_stOpStatus.stCycle.iTimeCnt++;

	if (!m_stOpCtrl.stZeroCtrl.bStageLast)
	{
		if (!m_stOpCtrl.stZeroCtrl.bWait)
        {
            SetEdmHolePower(TRUE,TRUE,FALSE);
            if(m_pEdm->WriteElecPara(&(m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[m_stOpStatus.stCycle.iOpPage]),"EdmHolePrune")==-1)
			{
				m_pOpFile->m_enOpFileErr = OP_FILE_ERR_ELEC;
				return FALSE;
			}
			m_stOpCtrl.iWaitCnt = EDM_OP_WAIT_COUNT;
			m_stOpCtrl.stZeroCtrl.bWait = TRUE;
			return FALSE;
		}

		memset(&cmd,0,sizeof(DIGIT_CMD));
		cmd.enAim = AIM_G90;
		cmd.enOrbit = ORBIT_G01;
		cmd.enCoor = m_pOpFile->m_enCoor;
		cmd.iFreq = MAC_INT_FREQ;
		cmd.stOp.bOpenPower = TRUE;
		cmd.stOp.enOpType = OP_HOLE_SING;
        cmd.stOp.iBackSense = m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[m_stOpStatus.stCycle.iOpPage].iBackSense;
        cmd.stOp.iFeedSense = m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[m_stOpStatus.stCycle.iOpPage].iFeedSense;

		cmd.stAxisDigit[cmd.iAxisCnt].iLabel = m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel;
		if (m_pEdm->m_stSysSet.stSetNoneLabel.bOpDir)
		{
            cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[m_stOpStatus.stCycle.iOpPage].iOpLen;
		} 
		else
		{
            cmd.stAxisDigit[cmd.iAxisCnt].iDistance = 0-m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[m_stOpStatus.stCycle.iOpPage].iOpLen;
		}
		cmd.iAxisCnt++;

		if (m_pEdm->EdmSendMovePara(&cmd))
		{
			m_stOpCtrl.stZeroCtrl.bWait = FALSE;
            m_stOpCtrl.stZeroCtrl.iOpLenSum = m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[m_stOpStatus.stCycle.iOpPage].iOpLen;
			m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
		}
	}

	return FALSE;
}

//单孔加工
unsigned char EDM_OP_HOLE::EdmHoleOpPage()
{
	int iSum =0;
    int iPage = 0;
    DIGIT_CMD cmd;
    Elec_Page elec;
    INFO_PRINT();
	if (m_stOpCtrl.stZeroCtrl.bStageLast)
    {
		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		m_stOpCtrl.stZeroCtrl.iOpLabelBasePos = m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iMachPos
            -  m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iWorkPosSet;
	    SetEdmHolePower(FALSE,FALSE,FALSE);
		return TRUE;	
	}

	m_stOpStatus.stCycle.iCycleIndex = 5;
	m_stOpStatus.stCycle.iTimeCnt++;

    m_stOpCtrl.stZeroCtrl.iOpLenSum = abs(m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iMachPos
                                      - m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iWorkPosSet);

    if (m_pEdm->m_stEdmComm.enMvStatus == RULE_MOVE_OVER)
    {
        for (int i=0;i<OP_HOLE_PAGE_MAX;i++)
        {
            iSum += m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[i].iOpLen;
            if (iSum > m_stOpCtrl.stZeroCtrl.iOpLenSum)
            {
                iPage = i;
                break;
            }

            if (iSum >= m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iOpLenAll)
            {
                iPage = i;
                iSum = m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iOpLenAll;
                break;
            }
        }

        if (m_stOpCtrl.stZeroCtrl.iOpLenSum >= m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iOpLenAll)
        {
            m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
            return FALSE;
        }

        if(m_pEdm->WriteElecPara(&(m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[iPage]),"EdmHoleOpPage_2")==-1)
        {
            m_pOpFile->m_enOpFileErr = OP_FILE_ERR_ELEC;
            return FALSE;
        }
        if (!EDM_OP::m_bSetPower)
        {
            SetEdmHolePower(TRUE,FALSE,FALSE);
            m_stOpCtrl.iWaitCnt = EDM_OP_WAIT_COUNT;
            return FALSE;
        }

        memset(&cmd,0,sizeof(DIGIT_CMD));
        cmd.enAim = AIM_G90;
        cmd.enOrbit = ORBIT_G01;
        cmd.enCoor = m_pOpFile->m_enCoor;

        cmd.stOp.bOpenPower = TRUE;
        cmd.stOp.enOpType = OP_HOLE_SING;
        cmd.stOp.iBackSense = m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[iPage].iBackSense;
        cmd.stOp.iFeedSense = m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[iPage].iFeedSense;

        cmd.stAxisDigit[cmd.iAxisCnt].iLabel = m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel;
        cmd.stAxisDigit[cmd.iAxisCnt].iDistance = iSum;
        if (!m_pEdm->m_stSysSet.stSetNoneLabel.bOpDir)
        {
            cmd.stAxisDigit[cmd.iAxisCnt].iDistance = 0-iSum;
        }
        cmd.iAxisCnt++;

        if ( m_pEdm->m_stSysSet.stSetNoneLabel.bPass)
        {
            memset(&elec,0,sizeof(Elec_Page));
            memcpy(&elec,&(m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[iPage]),sizeof(Elec_Page));
            elec.iServo = 55;
            if(m_pEdm->WriteElecPara(&elec,"EdmHoleOpPage_1")==-1)
            {
                m_pOpFile->m_enOpFileErr = OP_FILE_ERR_ELEC;
                return FALSE;
            }
        }

        if (m_pEdm->EdmSendMovePara(&cmd))
        {
            m_stOpStatus.stCycle.iOpPage = iPage;
            m_stOpCtrl.stZeroCtrl.iOpPageBak =  m_stOpStatus.stCycle.iOpPage;
            return FALSE;
        }
    }

	return FALSE;
}


unsigned char EDM_OP_HOLE::EdmHoleMillPage()
{
	int iSum =0;
	int iPage = 0;
	DIGIT_CMD cmd;
	DIGIT_CMD cmdDefault;
    CmdHandle* pCmdHandle;
	QString str;

	if (m_pEdm->m_stEdmComm.enMvStatus!= RULE_MOVE_OVER)
		return FALSE;

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{
		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;	
		if (m_stOpCtrl.stZeroCtrl.bMillLenEnough)
		{
			return TRUE;
		}
		m_stOpCtrl.stZeroCtrl.bMillLast = TRUE;
	}

	if (m_stOpCtrl.stZeroCtrl.bMillLast)
	{
		m_stOpCtrl.stZeroCtrl.bMillLast = FALSE;
		m_stOpStatus.iCmdIndex++;
	}

	if (m_stOpCtrl.stZeroCtrl.bMillPause)
	{
		m_stOpCtrl.stZeroCtrl.bMillPause = FALSE;
		m_stOpStatus.iCmdIndex++;
	}

	if (m_stOpStatus.iCmdIndex >= m_pOpFile->m_vCmd.size())
	{
		m_stOpStatus.iCmdIndex = 0;
	}

	m_stOpStatus.stCycle.iCycleIndex = 5;
	m_stOpStatus.stCycle.iTimeCnt++;

	if (m_pEdm->m_stEdmComm.enMvStatus== RULE_MOVE_OVER)
	{
		m_stOpCtrl.stZeroCtrl.iOpLenSum =  abs(m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iMachPos
			                                    - m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iWorkPosSet);
 
        if (m_stOpCtrl.stZeroCtrl.iOpLenSum >=  m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iOpLenAll
            && m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iOpLenAll >0)
		{
			m_stOpCtrl.stZeroCtrl.bMillLenEnough = TRUE;
			m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
			return TRUE;
		}

		if (m_stOpStatus.iCmdIndex>= m_pOpFile->m_vCmd.size())
		{
			m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
			return FALSE;
		}

		str = m_pOpFile->m_vCmd[m_stOpStatus.iCmdIndex];
        str = str.trimmed().toUpper();
        if (m_pOpFile->IsPauseCmd(str))
		{
			m_stOpStatus.bStart = FALSE;
			m_stOpStatus.stCycle.bPauseCmd = TRUE;
			m_stOpCtrl.stZeroCtrl.bMillPause = TRUE;
			m_stOpCtrl.stZeroCtrl.bMillLast = FALSE;
			return FALSE;
		} 
		else if (m_pOpFile->IsOverCmd(str))
		{
			m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
			m_stOpCtrl.stZeroCtrl.bMillLast = TRUE;
			return FALSE;
		}


		for (int i=0;i<OP_HOLE_PAGE_MAX;i++)
		{
            iSum += m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[i].iOpLen;
			if (iSum > m_stOpCtrl.stZeroCtrl.iOpLenSum)
			{
				iPage = i;
				break;
			}

            if (iSum >= m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iOpLenAll)
			{
				iPage = i;
                iSum = m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iOpLenAll;
				break;
			}
		}


        if(m_pEdm->WriteElecPara(&(m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[iPage]),"EdmHoleMillPage")==-1)
		{
			m_pOpFile->m_enOpFileErr = OP_FILE_ERR_ELEC;
			return FALSE;
		}
		if (!EDM_OP::m_bSetPower)
		{
			SetEdmHolePower(TRUE,FALSE,FALSE);
			m_stOpCtrl.iWaitCnt = EDM_OP_WAIT_COUNT;
			return FALSE;
		}

		memset(&cmdDefault,0,sizeof(DIGIT_CMD));
		cmdDefault.iFreq = MAC_INT_FREQ;

        pCmdHandle = new CmdHandle(FALSE,m_pOpFile->m_vCmd[m_stOpStatus.iCmdIndex],&cmd,&cmdDefault);
		delete pCmdHandle;
		if (cmd.enOrbit!=ORBIT_G00)
		{
			cmd.stOp.bOpenPower = TRUE;
			cmd.stOp.enOpType = OP_HOLE_MILL;
            cmd.stOp.iBackSense =  m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[iPage].iBackSense;
            cmd.stOp.iFeedSense =  m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[iPage].iFeedSense;
            cmd.stOp.iMillSense = m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iMillServo;
			//增加伺服轴
			cmd.stAxisDigit[cmd.iAxisCnt].iLabel = m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel;
			if (m_pEdm->m_stSysSet.stSetNoneLabel.bOpDir)
				cmd.stAxisDigit[cmd.iAxisCnt].iDistance = iSum;
			else
				cmd.stAxisDigit[cmd.iAxisCnt].iDistance = 0-iSum;
			cmd.iAxisCnt++;
		}

		cmd.enOrbit=ORBIT_G01;

		if (!m_pEdm->EdmSendMovePara(&cmd))
		{
			m_stOpCtrl.stZeroCtrl.bMillLast = FALSE;
		}
		else
		{
			m_stOpCtrl.stZeroCtrl.bMillLast = TRUE;
			m_stOpStatus.stCycle.iOpPage = iPage;
		}
	}

	return FALSE;
}

//重复加工
unsigned char EDM_OP_HOLE::EdmHoleRepeat()
{
    DIGIT_CMD cmd;
    INFO_PRINT();

	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;

    if (m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iRepeatLen<0 || m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iRepeatCount<0)
	{
		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		return TRUE;
	}

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{

		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		return TRUE;
	}

    if (m_stOpCtrl.stZeroCtrl.iRepeatCnt < m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iRepeatCount)
	{
		memset(&cmd,0,sizeof(DIGIT_CMD));		
		cmd.stOp.bOpenPower = FALSE;
		cmd.enAim = AIM_G90;
		cmd.enOrbit = ORBIT_G00;
		cmd.enCoor = m_pOpFile->m_enCoor;
		cmd.stAxisDigit[cmd.iAxisCnt].iLabel = m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel;

		if (!m_stOpCtrl.stZeroCtrl.bRepeatSec)
		{
			if (m_pEdm->m_stSysSet.stSetNoneLabel.bOpDir)
                cmd.stAxisDigit[cmd.iAxisCnt].iDistance =  m_stOpCtrl.stZeroCtrl.iOpLabelBasePos-m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iRepeatLen;
			else
                cmd.stAxisDigit[cmd.iAxisCnt].iDistance =  m_stOpCtrl.stZeroCtrl.iOpLabelBasePos+m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iRepeatLen;

			cmd.iAxisCnt++;
			if (m_pEdm->EdmSendMovePara(&cmd))
			{
				m_stOpCtrl.stZeroCtrl.bRepeatSec = TRUE;
			}
		} 
		else
		{
            if(m_pEdm->WriteElecPara(&(m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecPage[m_stOpCtrl.stZeroCtrl.iOpPageBak]),"EdmHoleRepeat")==-1)
			{
				m_pOpFile->m_enOpFileErr = OP_FILE_ERR_ELEC;
				return FALSE;
			}

			if (!EDM_OP::m_bSetPower)
			{
				SetEdmHolePower(TRUE,FALSE,FALSE);				
				m_stOpCtrl.iWaitCnt = EDM_OP_WAIT_COUNT;
				m_stOpCtrl.stZeroCtrl.bWait = TRUE;
				return FALSE;
			}

			cmd.enOrbit = ORBIT_G01;
			cmd.stAxisDigit[cmd.iAxisCnt].iDistance =  m_stOpCtrl.stZeroCtrl.iOpLabelBasePos;
			cmd.iAxisCnt++;
			if (m_pEdm->EdmSendMovePara(&cmd))
			{
				m_stOpCtrl.stZeroCtrl.bWait = FALSE;
				m_stOpCtrl.stZeroCtrl.bRepeatSec = FALSE;
				m_stOpCtrl.stZeroCtrl.iRepeatCnt++;
			}
		}
	} 
	else
	{
		m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
		return FALSE;
	}

	return FALSE;
}

//底部停留
unsigned char EDM_OP_HOLE::EdmHoleRootSleep()
{
    if (m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iBottomSleep>0)
	{
		if (!EDM_OP::m_bSetPower && m_stOpStatus.enOpType!= OP_HOLE_SIMULATE)
		{
			SetEdmHolePower(TRUE,FALSE,FALSE);
			m_stOpCtrl.iWaitCnt = EDM_OP_WAIT_COUNT;
			return FALSE;
		}

        if (m_stOpCtrl.stZeroCtrl.iSleepTimeCnt++ >= (m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iBottomSleep*1000)/OPERATE_TRREAD_SLEEP_TIME)
		{
			if (EDM_OP::m_bSetPower)
			{
				SetEdmHolePower(FALSE,FALSE,FALSE);
				m_stOpCtrl.iWaitCnt = EDM_OP_WAIT_COUNT;
			}

			return TRUE;
		}
		else
        {
			return FALSE;
		}
	}

	return TRUE;
}

//电极修整
unsigned char EDM_OP_HOLE::EdmHoleGo2AdjustPos()
{
	DIGIT_CMD cmd;
	QString strRec;
	QString strTmpRec;

	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;

	m_stOpStatus.stCycle.iCycleIndex = 5;

    memset(&cmd,0,sizeof(DIGIT_CMD));

    cmd.enAim = AIM_G90;
    cmd.enOrbit = ORBIT_G00;
    cmd.enCoor = m_pOpFile->m_enCoor;
    cmd.iFreq = MAC_INT_FREQ;

    cmd.stAxisDigit[cmd.iAxisCnt].iLabel = 0;
    cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_pEdm->m_stSysSet.stSetNoneLabel.iBlindPos_X
            - m_pEdm->m_stEdmComm.stMoveCtrlComm[0].iWorkPosSet;
    cmd.iAxisCnt++;
    cmd.stAxisDigit[cmd.iAxisCnt].iLabel = 1;
    cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_pEdm->m_stSysSet.stSetNoneLabel.iBlindPos_Y
            - m_pEdm->m_stEdmComm.stMoveCtrlComm[1].iWorkPosSet;
    cmd.iAxisCnt++;

    m_pEdm->EdmSendMovePara(&cmd);


	return FALSE;
}

unsigned char EDM_OP_HOLE::EdmHoleReturnOpPos()
{
	DIGIT_CMD cmd;
	QString strRec;
	QString strTmpRec;

	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;

	m_stOpStatus.stCycle.iCycleIndex = 5;


    memset(&cmd,0,sizeof(DIGIT_CMD));

    cmd.enAim = AIM_G90;
    cmd.enOrbit = ORBIT_G00;
    cmd.enCoor = m_pOpFile->m_enCoor;
    cmd.iFreq = MAC_INT_FREQ;

    cmd.stAxisDigit[cmd.iAxisCnt].iLabel = 0;
    cmd.stAxisDigit[cmd.iAxisCnt].iDistance = 0;
    cmd.iAxisCnt++;
    cmd.stAxisDigit[cmd.iAxisCnt].iLabel = 1;
    cmd.stAxisDigit[cmd.iAxisCnt].iDistance = 0;
    cmd.iAxisCnt++;

    m_pEdm->EdmSendMovePara(&cmd);


	return FALSE;
}

unsigned char EDM_OP_HOLE::EdmHoleGo2StartPos()
{
	DIGIT_CMD cmd;
	QString strRec;
	QString strTmpRec;

	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;


	m_stOpStatus.stCycle.iCycleIndex = 5;

    memset(&cmd,0,sizeof(DIGIT_CMD));

    cmd.enAim = AIM_G90;
    cmd.enOrbit = ORBIT_G00;
    cmd.enCoor = m_pOpFile->m_enCoor;
    cmd.iFreq = MAC_INT_FREQ/5;

    cmd.stAxisDigit[cmd.iAxisCnt].iLabel = m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel;
    cmd.stAxisDigit[cmd.iAxisCnt].iDistance = 0;
    cmd.iAxisCnt++;
    m_pEdm->EdmSendMovePara(&cmd);

	return FALSE;
}

//复位
unsigned char EDM_OP_HOLE::EdmHoleResetStartPos()
{
	DIGIT_CMD cmd;
	QString strRec;
	QString strTmpRec;

	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;

	m_stOpStatus.stCycle.iCycleIndex = 5;
    memset(&cmd,0,sizeof(DIGIT_CMD));
    m_pEdm->EdmSetProtect(FALSE);

    cmd.enAim = AIM_G92;
    cmd.enOrbit = ORBIT_G00;
    cmd.enCoor = m_pOpFile->m_enCoor;
    cmd.iFreq = MAC_INT_FREQ;

    cmd.stAxisDigit[cmd.iAxisCnt].iLabel = m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel;
    cmd.stAxisDigit[cmd.iAxisCnt].iDistance = 0;
    cmd.iAxisCnt++;
    m_pEdm->EdmSendMovePara(&cmd);

	return FALSE;
}

void EDM_OP_HOLE::CycleOver()
{
    INFO_PRINT();
	memset(&m_stOpCtrl.stZeroCtrl,0,sizeof(HOLE_ZERO_CTRL));
	m_stOpStatus.stCycle.iTimeCnt = 0;
    m_stOpStatus.stCycle.iTimeSec = 0;

	if (m_stOpStatus.enOpType!=OP_HOLE_SING)
    {
		if (m_stOpStatus.enOpType==OP_HOLE_MILL)
		{
			m_stOpStatus.iCmdIndex = 0;
			m_stOpStatus.bOpOver = TRUE;
		}
        m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iOpHoleIndex = m_stOpStatus.iCmdIndex;
	}

	if (m_stOpStatus.enOpType==OP_HOLE_SING || 
		 (m_stOpStatus.enOpType!=OP_HOLE_SING
           &&!m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.bContinueOp)
	   )
	{
		m_stOpStatus.bStart = FALSE;
		m_stOpStatus.stCycle.bPauseCmd = TRUE;
	}
	m_it = m_ListStage.begin();
}

void EDM_OP_HOLE::EdmHoleRecover()
{
	DIGIT_CMD cmd;
    EDM_OP::m_bSetPower = FALSE;
    m_pEdm->CloseHardWare();
    if(m_enOpType != OP_TYPE_NONE)
        m_pEdm->SaveElecElemDB(m_pOpFile->m_sFile);
	if (m_enOpType == OP_TYPE_NONE || EDM_OP::m_bInOp==FALSE)
	{
		return;
    }
    INFO_PRINT();
	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
	{
        while (!m_pEdm->EdmStopMove(FALSE))
		{
			for (int i=0;i<1000;i++)
			{
			}
		}

		while (!m_pEdm->GetEdmComm())
		{
			for (int i=0;i<1000;i++)
			{
			}
		}
	}
	
	memset(&cmd,0,sizeof(DIGIT_CMD));
	cmd.enAim = AIM_G92;
	cmd.enCoor = m_pOpFile->m_enCoor;
	cmd.enOrbit = ORBIT_G01;
	cmd.iFreq = m_iWholeFreq;
	for (int i=0;i<MAC_LABEL_COUNT;i++)
	{
		if (i==m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel)
		{
			continue;
		}
		cmd.stAxisDigit[cmd.iAxisCnt].iLabel = i;
		cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_pEdm->m_stEdmComm.stMoveCtrlComm[i].iMachPos- m_iWorkPos_All[i][(int)m_pOpFile->m_enCoor];
		cmd.iAxisCnt++;
	}

	while(!m_pEdm->EdmSendMovePara(&cmd))
	{
	}

	m_pEdm->ReSetWorkPosSetByIndex((int)m_pOpFile->m_enCoor,m_iWorkPos_All);	

    m_pEdm->EdmSetProtect(TRUE);
}


void EDM_OP_HOLE::SetEdmHolePower(unsigned char bPower,unsigned char bPrune,unsigned char bOtherClose)
{
	unsigned char bLowPump = FALSE;

	EDM_OP::m_bSetPower = bPower;	

	if (bPower)
	{
		bLowPump = m_pEdm->m_stSysSet.stSetNoneLabel.bLowPumpUnion;
	}
	else
	{
		bPrune = FALSE;
		if (bOtherClose)
		{
			bLowPump = FALSE;
		}
	}

	m_pEdm->EdmPower(bPower);
	m_pEdm->EdmPrune(bPrune);	
    m_pEdm->EdmSetShake(bPower);
	if (bPower || bOtherClose)
	{
		m_pEdm->EdmLowPump(bLowPump);
	}		
}


unsigned char EDM_OP_HOLE::ExteedTimeAlarm()
{
	int iCntMax =0;
	int iCntMin = 0;

	m_stOpStatus.stCycle.iTimeSec = (m_stOpStatus.stCycle.iTimeCnt * OPERATE_TRREAD_SLEEP_TIME)/1000;

	if(m_stOpStatus.enOpType==OP_HOLE_CHECK_C
	  || m_stOpStatus.enOpType==OP_HOLE_SIMULATE
	  || m_stOpStatus.enOpType==OP_HOLE_MILL)
	{
		return FALSE;
	}

    iCntMax = m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iTimeMax;
    iCntMin = m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iTimeMin;
    if (m_it==m_ListStage.end()  && m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iTimeMin>0 && (iCntMin>m_stOpStatus.stCycle.iTimeSec
        || iCntMax>iCntMin&& iCntMax<m_stOpStatus.stCycle.iTimeSec))
	{
        m_stOpStatus.errOp = OP_EXCEED_TIME;
		return TRUE;
	}

	return FALSE;
}

unsigned char EDM_OP_HOLE::PoleLenAlarm()
{
	int iLabel = m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel;

	if (m_pEdm->m_stSysSet.stSetNoneLabel.bOpDir)
	{
        if (m_pEdm->m_stEdmComm.stMoveCtrlComm[iLabel].iMachPos+m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iOpLenAll
			> m_pEdm->m_stEdmKpInt.stAxisCtrlKp[iLabel].stSoftPara.iTopPos)
		{
            m_stOpStatus.errOp = OP_LEN_POLE;
			return TRUE;
		}
	}
	else
	{
        if (m_pEdm->m_stEdmComm.stMoveCtrlComm[iLabel].iMachPos-m_pEdm->mp_ElecMan[m_pOpFile->m_sFile].stElecOral.iOpLenAll
			< m_pEdm->m_stEdmKpInt.stAxisCtrlKp[iLabel].stSoftPara.iLowerPos)
		{
            m_stOpStatus.errOp = OP_LEN_POLE;
			return TRUE;
		}
	}

	return FALSE;
}

int EDM_OP_HOLE::GetFirstCmdValOfCLabel()
{
	QString str;
	DIGIT_CMD  cmd;
    CmdHandle* pCmdHandle;
	int iLabel;

	for (int i=0;i<m_pOpFile->m_iCmdNum;i++)
	{
		str = m_pOpFile->m_vCmd[i];
        if (!m_pOpFile->IsPauseCmd(str)&& !m_pOpFile->IsOverCmd(str))
		{
			memset(&cmd,0,sizeof(DIGIT_CMD));
            pCmdHandle = new CmdHandle(FALSE,str,&cmd,&cmd);
			delete pCmdHandle;

			if (cmd.enOrbit == ORBIT_G01)
			{
				for (int j=0;j<cmd.iAxisCnt;j++)
				{
					iLabel = cmd.stAxisDigit[j].iLabel;
					if (iLabel==2)
					{
						return cmd.stAxisDigit[j].iDistance;
					}
				}
			}
		}
	}	

	return 360001;

}


void EDM_OP_HOLE::EdmOpGoHigh()
{
	DIGIT_CMD cmd;
	int iCnt = 300000;	

	if (m_pEdm->m_stSysSet.stSetNoneLabel.bOpDir)
	    iCnt = -300000;

	memset(&cmd,0,sizeof(DIGIT_CMD));
	cmd.enAim = AIM_G91;
	cmd.enOrbit = ORBIT_G00;
	cmd.enCoor = m_pOpFile->m_enCoor;
	cmd.iFreq = MAC_INT_FREQ;

	cmd.stAxisDigit[cmd.iAxisCnt].iLabel = m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel;
	cmd.stAxisDigit[cmd.iAxisCnt].iDistance = iCnt;
	cmd.iAxisCnt++;

	m_pEdm->EdmSendMovePara(&cmd);
}



unsigned char EDM_OP_HOLE::EdmOpMvAheadChkLabel()
{
	DIGIT_CMD cmd;
	CHECK_MODE enMode =CHECK_ELASTIC;
    INFO_PRINT();
	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{
		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		return TRUE;
	}

    m_pEdm->EdmStopMove(FALSE);

	if (enMode==CHECK_ELEC)
	{
        if (!m_pEdm->m_stEdmShowData.stEdmInterfaceIn.btI144 & 0x08)
		{
			m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
			return FALSE;
		}
	} 
	else if (enMode==CHECK_ELASTIC)
	{
        m_stOpCtrl.stZeroCtrl.iCheckPos = 0;
		if (m_stOpCtrl.stZeroCtrl.iCheckPos >=1980)
		{
			m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
			return FALSE;
		}
	}
	else
	{
		m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
		return FALSE;
	}

	memset(&cmd,0,sizeof(DIGIT_CMD));
	cmd.enAim = AIM_G91;
	cmd.enOrbit = ORBIT_G00;
	cmd.enCoor = m_pOpFile->m_enCoor;
	cmd.iFreq = 500;
	cmd.stAxisDigit[cmd.iAxisCnt].iLabel = 0;
	cmd.stAxisDigit[cmd.iAxisCnt].iDistance +=100;
	cmd.iAxisCnt++;
	m_pEdm->EdmSendMovePara(&cmd);
	return FALSE;
}



unsigned char EDM_OP_HOLE::EdmOpMvWaitChkLabel()
{
	int iVal;
	CHECK_MODE enMode =CHECK_ELASTIC;

	if (++m_stOpCtrl.stZeroCtrl.iCheckTimeCnt<10)
	{
		return FALSE;
	}

	if (enMode==CHECK_ELEC)
	{
		iVal = m_pEdm->m_stEdmComm.stMoveCtrlComm[0].iMachPos;
	} 
	else if (enMode==CHECK_ELASTIC)
	{
        m_stOpCtrl.stZeroCtrl.iCheckPos = 0;
		iVal = m_pEdm->m_stEdmComm.stMoveCtrlComm[0].iMachPos + m_stOpCtrl.stZeroCtrl.iCheckPos;
	}
	else
	{
		iVal = m_pEdm->m_stEdmComm.stMoveCtrlComm[0].iMachPos;
	}	
	
	m_pEdm->SetValMakeUp(m_stOpStatus.iCmdIndex,iVal);

	return TRUE;
}


unsigned char EDM_OP_HOLE::EdmOpMvBackChkLabel()
{
	DIGIT_CMD cmd;
	CHECK_MODE enMode =CHECK_ELASTIC;
    INFO_PRINT();
	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
	{
		return FALSE;
	}

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{
		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		return TRUE;
	}	

	memset(&cmd,0,sizeof(DIGIT_CMD));
	cmd.enAim = AIM_G90;
	cmd.enOrbit = ORBIT_G00;
	cmd.enCoor = m_pOpFile->m_enCoor;
	cmd.bNoCheck = TRUE;
	cmd.iFreq = 10000;
	cmd.stAxisDigit[cmd.iAxisCnt].iLabel = 0;

	if (enMode==CHECK_ELEC)
	{
		cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_pEdm->m_stEdmComm.stMoveCtrlComm[0].iMachPos
			- m_pEdm->m_stEdmComm.stMoveCtrlComm[0].iWorkPosSet 
            - 7000;
	} 
	else if (enMode==CHECK_ELASTIC)
	{
		cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_pEdm->m_stEdmComm.stMoveCtrlComm[0].iMachPos
			- m_pEdm->m_stEdmComm.stMoveCtrlComm[0].iWorkPosSet 
			- m_stOpCtrl.stZeroCtrl.iCheckPos - 5000;
	}
	else
	{
		cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_pEdm->m_stEdmComm.stMoveCtrlComm[0].iMachPos
			- m_pEdm->m_stEdmComm.stMoveCtrlComm[0].iWorkPosSet 
			- 7000;
	}
	cmd.iAxisCnt++;
	if (m_pEdm->EdmSendMovePara(&cmd))
	{
		m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
	}
	return FALSE;
}
