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
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleUp2Safe);	
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleSynchro);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleLocation);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleDownFromSafe);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleZeroAdjust);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHolePrune);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleOpPage);
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
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleUp2Safe);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleSynchro);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleLocation);
		m_ListStage.push_back(&EDM_OP_HOLE::EdmHoleDownFromSafe);
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

void EDM_OP_HOLE::EdmOpSetTest(BOOL bTest)
{
	memset(&m_stOpCtrl,0,sizeof(HOLE_CTRL));
	if (bTest)
	{
		m_stOpStatus.enOpType = OP_HOLE_CHECK_C;		
	}
	else
	{
		m_stOpStatus.enOpType = m_enOpType;		
	}
	EdmHoleOpTypeInit();
	m_pEdm->ClearMakeUpVal();
	m_stOpStatus.bCheck_C_Over = TRUE;
}


void EDM_OP_HOLE::EdmOpSetStart(BOOL bStart)
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
		m_stOpStatus.bCheck_C_Over = TRUE;

		if (m_stOpStatus.bOpOver 
			|| m_stOpStatus.iCmdIndex>= m_pOpFile->m_iCmdNum
			|| m_stOpStatus.iCmdIndex<0)
		{
			memset(&m_stOpCtrl,0,sizeof(HOLE_CTRL));
			memset(&m_stOpStatus,0,sizeof(OP_STATUS));
			m_stOpStatus.enOpType = enOpType;			
			m_stOpCtrl.iWaitCnt = 0;
		}	

		if (!m_stOpStatus.stCycle.bCycleStart)
		{
			m_stOpStatus.iCmdIndex = m_pOpFile->m_mpElecMan[EDM::m_strElecDefault].stElecOral.iOpHoleIndex-1;
			m_stOpStatus.iCmdIndex = max(m_stOpStatus.iCmdIndex,0);
			m_stOpStatus.iCmdIndex = min(m_stOpStatus.iCmdIndex,m_pOpFile->m_iCmdNum);
			memset(&m_stOpStatus.stCycle,0,sizeof(STATUS_NEW_CYCYLE));			
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
		m_pOpFile->GetLastElecName(m_stOpStatus.iCmdIndex);
        m_stOpStatus.bCheck_C_Over = TRUE;
	}
	else
	{
		m_stOpStatus.bStart = FALSE;
		EDM_OP::m_bInOp = FALSE; 
		m_stOpStatus.stCycle.bPauseCmd = TRUE;
		while (!m_pEdm->EdmStopMove(false))
		{
		}
		m_pEdm->EdmSetProtect(true);
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
		if (m_stOpStatus.iCmdIndex != m_pOpFile->m_mpElecMan[EDM::m_strElecDefault].stElecOral.iOpHoleIndex-1)
		{
			m_pOpFile->SetEdmElecIndex(m_stOpStatus.iCmdIndex+1);
		}

		if (!m_pEdm->m_bOffLine)
		{
			if (m_pOpFile)
			{
				m_stOpStatus.enErrAll.errFile = m_pOpFile->m_enOpFileErr;
			}
			
			if (EdmOpErr())
			{
				EdmHoleRecover();
				if (m_stOpStatus.enErrAll.errOp==OP_LEN_POLE)
				{
					EdmOpGoHigh();
				}
				return;
			}
		}		

		if (m_stOpStatus.bStart)
		{
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
}

void EDM_OP_HOLE::SetAllErr()
{
	m_stOpStatus.enErrAll.errOp = OP_NO_ERR;
	if (m_pOpFile)
	{
		m_stOpStatus.enErrAll.errFile = m_pOpFile->m_enOpFileErr;
	}
	else
	{
		m_stOpStatus.enErrAll.errFile = OP_FILE_NO_EXIST;
	}
}


void EDM_OP_HOLE::EdmOpOver()
{	
	m_pEdm->CloseHardWare();
	EdmHoleRecover();
	EDM_OP::m_bInOp = FALSE;
	EDM_OP::m_bStartCount = FALSE;
	EDM_OP::m_bSetPower = FALSE;
}

void EDM_OP_HOLE::EdmOpStageRestart()
{
	while (!m_pEdm->EdmStopMove(false))
	{
	}
	m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
}


void EDM_OP_HOLE::EdmHoleCarry()
{
	fEdmOpStage pStageCarry;
	QString strRec;

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
	QString str;

	if (m_stOpStatus.iCmdIndex>=m_pOpFile->m_iCmdNum || m_stOpStatus.iCmdIndex<0)
	{
		EdmHoleOverProcess();	
		return;
	}

	str = m_pOpFile->m_vCmd[m_stOpStatus.iCmdIndex];

	if (m_pOpFile->IsPauseCmd(str))
		EdmHolePauseProcess();
	else if (m_pOpFile->IsOverCmd(str))
		EdmHoleOverProcess();
	else if (m_pOpFile->IsStrElecCmd(str))
		EdmHoleElecProcess();
	else if (m_pOpFile->IsMillFile(str))
		EdmHoleMillFileProcess();
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
	m_stOpStatus.bCheck_C_Over = TRUE;
	m_pOpFile->SetEdmElecIndex(1);
	EdmHoleRecover();
}

void EDM_OP_HOLE::EdmHoleElecProcess()
{
	//m_strElec = m_vCmd[m_stOpStatus.iCmdIndex];
	m_pOpFile->GetLastElecName(m_stOpStatus.iCmdIndex);
	CycleOver();
}

void EDM_OP_HOLE::EdmHoleMillFileProcess()
{
	DIGIT_CMD cmd;
	QString strRec;
	QString str=m_pOpFile->m_vCmd[m_stOpStatus.iCmdIndex];
	m_stOpStatus.bNewOp = TRUE;
	m_stOpStatus.pNewHoleOp = new EDM_OP_HOLE(OP_HOLE_MILL);
    ((EDM_OP*)m_stOpStatus.pNewHoleOp)->SetEdmOpFile(m_sPath,str);
	((EDM_OP*)m_stOpStatus.pNewHoleOp)->m_pOpFile->PlusDigit2Cmd();

	memset(&cmd,0,sizeof(DIGIT_CMD));
	cmd.enAim = AIM_G92;
	cmd.enCoor =  ((EDM_OP*)m_stOpStatus.pNewHoleOp)->m_pOpFile->m_enCoor;
	cmd.enOrbit = ORBIT_G01;
	for (int i=0;i<MAC_LABEL_COUNT;i++)
	{
		if (i==m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel)
		{
			continue;
		}
		cmd.stAxisDigit[cmd.iAxisCnt].iLabel = i;
		cmd.iAxisCnt++;
	}

	while (!m_pEdm->EdmSendMovePara(&cmd))
	{
		for (int j=0;j<10000;j++)
		{
		}
	}

	for (int j=0;j<10000;j++)
	{
	}

	((EDM_OP*)m_stOpStatus.pNewHoleOp)->m_stOpStatus.iCmdIndex = 0;
	((EDM_OP*)m_stOpStatus.pNewHoleOp)->m_pOpFile->SetEdmElecIndex(1);
	((EDM_OP*)m_stOpStatus.pNewHoleOp)->m_pOpFile->GetLastElecName(0);
	m_it=m_ListStage.end();
	m_stOpCtrl.bSynchro = TRUE;
}


void EDM_OP_HOLE::EdmHoleMvCmdProcess()
{
	QString str;
	DIGIT_CMD cmdDefault;
    CmdHandle* pCmdHandle;

	memset(&cmdDefault,0,sizeof(DIGIT_CMD));
	memset(&m_stMvCmd,0,sizeof(DIGIT_CMD));
	str = m_pOpFile->m_vCmd[m_stOpStatus.iCmdIndex];
    pCmdHandle = new CmdHandle(FALSE,str,&m_stMvCmd,&cmdDefault);
	m_stMvCmd.iFreq = min(m_stMvCmd.iFreq,m_iWholeFreq);
	m_stMvCmd.stOp.bShortDis = TRUE;
	delete pCmdHandle;
}

//主轴回升
BOOL EDM_OP_HOLE::EdmHoleRise()
{
	BOOL bRise = FALSE;
	DIGIT_CMD cmd;
	QString strRec;
	QString strTmpRec = " ";
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
        if ((m_pEdm->m_stSysSet.stSetNoneLabel.bOpDir && iPos>m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iRisePos)
            || (!m_pEdm->m_stSysSet.stSetNoneLabel.bOpDir && iPos<m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iRisePos))
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
			cmd.stAxisDigit[0].iDistance = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iRisePos;
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

BOOL EDM_OP_HOLE::EdmHoleUp2Safe()
{
	DIGIT_CMD cmd;
	QString strRec;
	QString strTmpRec;

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

	if (!m_stOpCtrl.stZeroCtrl.bStageLast && m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel==4)
	{	
		memset(&cmd,0,sizeof(DIGIT_CMD));
		cmd.enAim = AIM_G90;
		cmd.enCoor = m_pOpFile->m_enCoor;
		cmd.enOrbit = ORBIT_G01;
		cmd.iFreq = MAC_INT_FREQ;
		cmd.stAxisDigit[cmd.iAxisCnt].iLabel = m_pEdm->m_stSysSet.iAxistLabel;
		cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iSafePos;
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


BOOL EDM_OP_HOLE::EdmHoleDownFromSafe()
{
	DIGIT_CMD cmd;
	QString strRec;
	QString strTmpRec;

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
			CycleOver();
		}		
		return TRUE;
	}

	if (!m_stOpCtrl.stZeroCtrl.bStageLast && m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel==4)
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

BOOL EDM_OP_HOLE::EdmHoleLocation()
{
	int iLabel;
	int iVal;
	BOOL bExist = FALSE;
	BOOL bAddCycleMeasVal = FALSE;
    DIGIT_CMD cmd2Send;
	BOOL bEmptyMove=FALSE;

	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{
        m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		if (m_stOpCtrl.stZeroCtrl.bSafeLabelExist)
		{
			if (m_pEdm->m_stSysSet.iAxistLabel>=0 && m_pEdm->m_stSysSet.iAxistLabel<MAC_LABEL_COUNT)
			{
				m_iSafeLabelMacPos = m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.iAxistLabel].iMachPos;
			}			
		}
		
		return TRUE;
	}

	m_stOpStatus.stCycle.iCycleIndex = 2;

	if (!m_stOpCtrl.stZeroCtrl.bStageLast)
    {
		m_stOpCtrl.bSynchro = FALSE;

		for (int i=0;i<m_stMvCmd.iAxisCnt;i++)
		{
			iLabel = m_stMvCmd.stAxisDigit[i].iLabel;
			if (iLabel == m_pEdm->m_stSysSet.iAxistLabel)
			{
				m_stOpCtrl.stZeroCtrl.bSafeLabelExist = TRUE;
			}

			if (iLabel==2 
				&& m_pEdm->HasMakeUpVal() 
				&& m_pEdm->m_stSysSet.stSetNoneLabel.bCycleMeasure 
				&& m_stOpStatus.enOpType!=OP_HOLE_CHECK_C)
			{
				bAddCycleMeasVal = m_pEdm->GetMakeUpVal(m_stOpStatus.iCmdIndex,&iVal);
			}
		}

		memcpy(&cmd2Send,&m_stMvCmd,sizeof(DIGIT_CMD));

		if (bAddCycleMeasVal && m_stOpStatus.enOpType!=OP_HOLE_CHECK_C)
		{
			bExist = FALSE;
			m_stOpCtrl.bSynchro = TRUE;
			for (int i=0;i<m_stMvCmd.iAxisCnt;i++)
			{
				iLabel = m_stMvCmd.stAxisDigit[i].iLabel;
				if (iLabel==0)
				{
					bExist = TRUE;
					cmd2Send.stAxisDigit[i].iDistance +=iVal;
					break;
				}
			}

			if (!bExist)
			{
				cmd2Send.stAxisDigit[cmd2Send.iAxisCnt].iLabel = 0;
				cmd2Send.stAxisDigit[cmd2Send.iAxisCnt].iDistance = m_pEdm->m_stEdmComm.stMoveCtrlComm[0].iMachPos
					- m_pEdm->m_stEdmComm.stMoveCtrlComm[0].iWorkPosSet + iVal;
				cmd2Send.iAxisCnt++;
			}
		}

		if (cmd2Send.enOrbit == ORBIT_G00)
		{
			bEmptyMove = TRUE;
		}

		while (!m_pEdm->EdmSetProtect(true))
		{
		}
		cmd2Send.enOrbit = ORBIT_G01;
		if(m_pEdm->EdmSendMovePara(&cmd2Send))
		{
			m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
			m_stOpCtrl.stZeroCtrl.bEmptyMove = bEmptyMove;
			return FALSE;
		}
	}	

	return FALSE;	
}

BOOL EDM_OP_HOLE::EdmHoleSynchro()
{
	DIGIT_CMD cmd;
	DIGIT_CMD cmd2Send;
    CmdHandle* pCmdHandle;
	int iWorkPos[MAC_LABEL_COUNT];
	int iLabel;
	int iCmdIndex;
	QString str;
	QString strRec;
	QString strTmpRec;

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

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{
        m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		return TRUE;
	}

	
	m_stOpStatus.stCycle.iCycleIndex = 2;

	if (!m_stOpCtrl.stZeroCtrl.bStageLast)
    {
		iCmdIndex = m_stOpStatus.iCmdIndex+1;
		while (iCmdIndex<m_pOpFile->m_iCmdNum)
		{
			str = m_pOpFile->m_vCmd[iCmdIndex];
			if (!m_pOpFile->IsPauseCmd(str) 
				&& !m_pOpFile->IsStrElecCmd(str) 
				&& !m_pOpFile->IsMillFile(str))
			{
				break;
			}
			iCmdIndex++;
		}
		if (iCmdIndex >= m_pOpFile->m_iCmdNum)
		{
			return TRUE;
		}

		str = m_pOpFile->m_vCmdLoc[iCmdIndex];
		memset(&cmd,0,sizeof(DIGIT_CMD));
        pCmdHandle = new CmdHandle(FALSE,str,&cmd,&cmd);
		delete pCmdHandle;

		while (!m_pEdm->EdmSetProtect(true))
		{
		}

		m_pEdm->GetWorkPosSetByIndex((int)cmd.enCoor,iWorkPos);		
		for (int k=0;k<cmd.iAxisCnt;k++)
		{
			iLabel = cmd.stAxisDigit[k].iLabel;
			if (iLabel==m_pEdm->m_stSysSet.iAxistLabel)
			{
				m_iSafeLabelMacPos = cmd.stAxisDigit[k].iDistance;
			}
			cmd.stAxisDigit[k].iDistance = cmd.stAxisDigit[k].iDistance 
				- m_pEdm->m_stEdmComm.stMoveCtrlComm[iLabel].iMachPos;			
		}
		cmd.enAim = AIM_G91;

		memcpy(&cmd2Send,&cmd,sizeof(DIGIT_CMD));
		cmd2Send.stOp.bShortDis = TRUE;

		//过滤没有使用的轴
		cmd2Send.iAxisCnt = 0;
		for (int k=0;k<cmd.iAxisCnt;k++)
		{
			iLabel = cmd.stAxisDigit[k].iLabel;
			if (m_pEdm->m_stSysSet.bSetUse[iLabel])
			{
				cmd2Send.stAxisDigit[cmd2Send.iAxisCnt].iLabel = cmd.stAxisDigit[k].iLabel;
				cmd2Send.stAxisDigit[cmd2Send.iAxisCnt].iDistance = cmd.stAxisDigit[k].iDistance;
				cmd2Send.iAxisCnt++;
			}
		}
		
		if(m_pEdm->EdmSendMovePara(&cmd2Send))
		{
			m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
			return FALSE;
		}	
	}	

	return FALSE;
}

//电极对零
BOOL EDM_OP_HOLE::EdmHoleZeroAdjust_Sing()
{
	DIGIT_CMD cmd;
	QString strRec;
	QString strTmpRec;

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


BOOL EDM_OP_HOLE::EdmHoleZeroAdjust()
{
	Elec_Page stElec;
	DIGIT_CMD cmd;
	QString strRec;
	QString strTmpRec;

	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{
		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		m_stOpCtrl.stZeroCtrl.bWait = FALSE;

		if (PoleLenAlarm() && !m_pEdm->m_bOffLine)
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
			memset(&stElec,0,sizeof(Elec_Page));
			stElec.iTon  = 2;
			stElec.iToff = 80;
			stElec.iElecLow = 1;
			stElec.iElecHigh = 0;
			stElec.iCap = 0;
			stElec.iServo = 75;
			stElec.iRotSpeed = 1;
			m_pEdm->WriteElecPara(&stElec,"EdmHoleZeroAdjust");

			m_stOpCtrl.iWaitCnt = 1;
			if (!EDM_OP::m_bSetPower)
			{
				SetEdmHolePower(TRUE,FALSE,FALSE);
				m_stOpCtrl.iWaitCnt = EDM_OP_WAIT_COUNT;				
			}
			return FALSE;
		}

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
BOOL EDM_OP_HOLE::EdmHolePrune()
{
	DIGIT_CMD cmd;
	QString strRec;
	QString strTmpRec;
	
	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
	   return FALSE;	

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
    {
		m_stOpStatus.stCycle.stPassChart.bClear = TRUE;
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
			m_pOpFile->GetLastElecName(m_stOpStatus.iCmdIndex);
			if(m_pEdm->WriteElecPara(&(m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[m_stOpStatus.stCycle.iOpPage]),"EdmHolePrune")==-1)
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
		cmd.stOp.iBackSense = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[m_stOpStatus.stCycle.iOpPage].iBackSense;
		cmd.stOp.iFeedSense = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[m_stOpStatus.stCycle.iOpPage].iFeedSense;

		cmd.stAxisDigit[cmd.iAxisCnt].iLabel = m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel;
		if (m_pEdm->m_stSysSet.stSetNoneLabel.bOpDir)
		{
			cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[m_stOpStatus.stCycle.iOpPage].iOpLen;
		} 
		else
		{
			cmd.stAxisDigit[cmd.iAxisCnt].iDistance = 0-m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[m_stOpStatus.stCycle.iOpPage].iOpLen;
		}
		cmd.iAxisCnt++;

		if (m_pEdm->EdmSendMovePara(&cmd))
		{
			m_stOpCtrl.stZeroCtrl.bWait = FALSE;
			m_stOpCtrl.stZeroCtrl.iOpLenSum = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[m_stOpStatus.stCycle.iOpPage].iOpLen;
			m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
		}
	}

	return FALSE;
}

//单孔加工
BOOL EDM_OP_HOLE::EdmHoleOpPage()
{
	int iSum =0;
	int iPage = 0;
	int iPassLen;
	DIGIT_CMD cmd;
	int iToffIndex;
	Elec_Page elec;
	QString strRec;
	QString strTmpRec;

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
    {
		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		m_stOpCtrl.stZeroCtrl.iOpLabelBasePos = m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iMachPos
			-  m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iWorkPosSet;	
		m_stOpStatus.stCycle.stPassChart.bRealTimeIn = FALSE;		
	    SetEdmHolePower(FALSE,FALSE,FALSE);
		return TRUE;	
	}

	m_stOpStatus.stCycle.iCycleIndex = 5;
	m_stOpStatus.stCycle.iTimeCnt++;

	if (m_pEdm->m_stSysSet.stSetNoneLabel.bPass 
		&& (m_stOpStatus.enOpType==OP_HOLE_SING|| m_stOpStatus.enOpType==OP_HOLE_PROGRAME))
	{
		EdmHolePassCtl();
	}

	if(m_stOpCtrl.stZeroCtrl.stPassCtl.enPassMode == PASS_HIGH)
	{
		if (!m_stOpCtrl.stZeroCtrl.stPassCtl.bHighModeStop)
		{
			while (!m_pEdm->EdmStopMove(false))
			{
			}
		}

		m_stOpCtrl.stZeroCtrl.stPassCtl.bHighModeStop = TRUE;
		if (++m_stOpCtrl.stZeroCtrl.stPassCtl.iHighTimeCnt  > 200/OPERATE_TRREAD_SLEEP_TIME)
		{			
			m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
			return FALSE;
		}
	}

	if (m_stOpCtrl.stZeroCtrl.stPassCtl.enPassMode == PASS_NONE)
	{
		m_stOpCtrl.stZeroCtrl.iOpLenSum = abs(m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iMachPos
			                              - m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iWorkPosSet);

		if (m_pEdm->m_stEdmComm.enMvStatus == RULE_MOVE_OVER
			|| (!m_stOpCtrl.stZeroCtrl.stPassCtl.bEntrySet && m_stOpCtrl.stZeroCtrl.stPassCtl.bEntryOver))
		{
			if (!m_stOpCtrl.stZeroCtrl.stPassCtl.bEntrySet && m_stOpCtrl.stZeroCtrl.stPassCtl.bEntryOver) 
			{
				m_stOpCtrl.stZeroCtrl.stPassCtl.bEntrySet = TRUE;
				while (!m_pEdm->EdmStopMove(false))
				{
				}
			}

			for (int i=0;i<OP_HOLE_PAGE_MAX;i++)
			{
				iSum += m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[i].iOpLen;
				if (iSum > m_stOpCtrl.stZeroCtrl.iOpLenSum)
				{
					iPage = i;
					break;
				}
				
				if (iSum >= m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iOpLenAll)
				{
					iPage = i;
					iSum = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iOpLenAll;
					break;
				}
			}

			if (m_stOpCtrl.stZeroCtrl.iOpLenSum >= m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iOpLenAll)
			{
				m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
				return FALSE;
			}

			if(m_pEdm->WriteElecPara(&(m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[iPage]),"EdmHoleOpPage_2")==-1)
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
			cmd.stOp.iBackSense = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[iPage].iBackSense;
			cmd.stOp.iFeedSense = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[iPage].iFeedSense;			

			cmd.stAxisDigit[cmd.iAxisCnt].iLabel = m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel;
			cmd.stAxisDigit[cmd.iAxisCnt].iDistance = iSum;
			if (!m_pEdm->m_stSysSet.stSetNoneLabel.bOpDir)
			{
				cmd.stAxisDigit[cmd.iAxisCnt].iDistance = 0-iSum;
			}
			cmd.iAxisCnt++;

			if ( m_pEdm->m_stSysSet.stSetNoneLabel.bPass &&
				(!m_stOpCtrl.stZeroCtrl.stPassCtl.bEntryOver|| (m_stOpCtrl.stZeroCtrl.stPassCtl.bPole &&m_pEdm->m_stSysSet.stSetNoneLabel.iPoleMode)))
			{
				memset(&elec,0,sizeof(Elec_Page));
				memcpy(&elec,&(m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[iPage]),sizeof(Elec_Page));
                GetElecToff(elec.iToff,&iToffIndex);
				iToffIndex += m_pEdm->m_stSysSet.stSetNoneLabel.iToff;
                elec.iToff = GetElecToffByIndex(iToffIndex);
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
	}
	else
	{
		if (m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iMachPos
			- m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iWorkPosSet == m_stOpCtrl.stZeroCtrl.stPassCtl.iPassRelWork)
		{
			m_stOpCtrl.stZeroCtrl.bStageLast = TRUE;
			return FALSE;
		}

		if (m_stOpCtrl.stZeroCtrl.stPassCtl.enPassMode==PASS_LOW && !m_stOpCtrl.stZeroCtrl.stPassCtl.bPassLowSet)
		{
			if (!EDM_OP::m_bSetPower)
			{
				SetEdmHolePower(TRUE,FALSE,FALSE);
				m_stOpCtrl.iWaitCnt = EDM_OP_WAIT_COUNT;
				return FALSE;
			}

			if (m_pEdm->EdmStopMove(false))
			{
				iPassLen =  m_pEdm->m_stSysSet.stSetNoneLabel.iPassLen;
				iPassLen = max(10,iPassLen);

				memset(&cmd,0,sizeof(DIGIT_CMD));
				cmd.enAim = AIM_G90;
				cmd.enOrbit = ORBIT_G01;
				cmd.enCoor = m_pOpFile->m_enCoor;
				cmd.stOp.bOpenPower = TRUE;
				cmd.stOp.iBackSense = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[iPage].iBackSense;
				cmd.stOp.iFeedSense = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[iPage].iFeedSense;
				cmd.stOp.enOpType = OP_HOLE_SING;

				cmd.stAxisDigit[cmd.iAxisCnt].iLabel = m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel;
				cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iMachPos
					- m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iWorkPosSet;
				if (m_pEdm->m_stSysSet.stSetNoneLabel.bOpDir)
				{
					cmd.stAxisDigit[cmd.iAxisCnt].iDistance += iPassLen;
				} 
				else
				{
					cmd.stAxisDigit[cmd.iAxisCnt].iDistance -= iPassLen;
				}
				cmd.iAxisCnt++;

				memcpy(&elec,&(m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[m_stOpStatus.stCycle.iOpPage]),sizeof(Elec_Page));
                GetElecToff(elec.iToff,&iToffIndex);
				iToffIndex += m_pEdm->m_stSysSet.stSetNoneLabel.iToff;
                elec.iToff = GetElecToffByIndex(iToffIndex);
                GetElecCap(elec.iCap,&iToffIndex);
				iToffIndex += m_pEdm->m_stSysSet.stSetNoneLabel.iCap;
                elec.iCap = GetElecCapByIndex(iToffIndex);
				elec.iRotSpeed += m_pEdm->m_stSysSet.stSetNoneLabel.iRotate;
				elec.iServo += m_pEdm->m_stSysSet.stSetNoneLabel.iSifu;
				if(m_pEdm->WriteElecPara(&elec,"EdmHoleOpPage_3")==-1)
				{
					m_pOpFile->m_enOpFileErr = OP_FILE_ERR_ELEC;
					return FALSE;
				}

				if (m_pEdm->EdmSendMovePara(&cmd))
				{

					m_stOpCtrl.stZeroCtrl.stPassCtl.bPassLowSet = TRUE;
					m_stOpCtrl.stZeroCtrl.stPassCtl.iPassRelWork = cmd.stAxisDigit[0].iDistance;
					return FALSE;
				}
			}
		}
	}

	return FALSE;
}


BOOL EDM_OP_HOLE::EdmHoleMillPage()
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
		if (m_pEdm->m_bOffLine)
		{
			m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
			m_stOpStatus.stCycle.bPauseCmd = TRUE;
			m_stOpStatus.bStart = FALSE;
			return TRUE;
		}
	}

	m_stOpStatus.stCycle.iCycleIndex = 5;
	m_stOpStatus.stCycle.iTimeCnt++;

	if (m_pEdm->m_stEdmComm.enMvStatus== RULE_MOVE_OVER)
	{
		m_stOpCtrl.stZeroCtrl.iOpLenSum =  abs(m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iMachPos
			                                    - m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iWorkPosSet);
 
		if (m_stOpCtrl.stZeroCtrl.iOpLenSum >=  m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iOpLenAll
			&& m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iOpLenAll >0)
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

		m_pOpFile->GetLastElecName(m_stOpStatus.iCmdIndex);
		str = m_pOpFile->m_vCmd[m_stOpStatus.iCmdIndex];
        str = str.trimmed().toUpper();
		if (m_pOpFile->IsStrElecCmd(str))
		{
			m_stOpCtrl.stZeroCtrl.bMillLast = TRUE;
			return FALSE;
		}
		else if (m_pOpFile->IsPauseCmd(str))
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
			iSum += m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[i].iOpLen;
			if (iSum > m_stOpCtrl.stZeroCtrl.iOpLenSum)
			{
				iPage = i;
				break;
			}

			if (iSum >= m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iOpLenAll)
			{
				iPage = i;
				iSum = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iOpLenAll;
				break;
			}
		}


		if(m_pEdm->WriteElecPara(&(m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[iPage]),"EdmHoleMillPage")==-1)
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
			cmd.stOp.iBackSense =  m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[iPage].iBackSense;
			cmd.stOp.iFeedSense =  m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[iPage].iFeedSense;
			cmd.stOp.iMillSense = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iMillServo;
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
BOOL EDM_OP_HOLE::EdmHoleRepeat()
{
	DIGIT_CMD cmd;
	QString strRec;
	QString strTmpRec;

	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;

	if (m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iRepeatLen<0 || m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iRepeatCount<0)
	{
		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		return TRUE;
	}

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{

		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		return TRUE;
	}

	if (m_stOpCtrl.stZeroCtrl.iRepeatCnt < m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iRepeatCount)
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
				cmd.stAxisDigit[cmd.iAxisCnt].iDistance =  m_stOpCtrl.stZeroCtrl.iOpLabelBasePos-m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iRepeatLen;
			else
				cmd.stAxisDigit[cmd.iAxisCnt].iDistance =  m_stOpCtrl.stZeroCtrl.iOpLabelBasePos+m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iRepeatLen;

			cmd.iAxisCnt++;
			if (m_pEdm->EdmSendMovePara(&cmd))
			{
				m_stOpCtrl.stZeroCtrl.bRepeatSec = TRUE;
			}
		} 
		else
		{
			if(m_pEdm->WriteElecPara(&(m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[m_stOpCtrl.stZeroCtrl.iOpPageBak]),"EdmHoleRepeat")==-1)
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
BOOL EDM_OP_HOLE::EdmHoleRootSleep()
{
	QString strRec;
	QString strTmpRec;

	if (m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iBottomSleep>0)
	{
		if (!EDM_OP::m_bSetPower && m_stOpStatus.enOpType!= OP_HOLE_SIMULATE)
		{
			SetEdmHolePower(TRUE,FALSE,FALSE);
			m_stOpCtrl.iWaitCnt = EDM_OP_WAIT_COUNT;
			return FALSE;
		}

		if (m_stOpCtrl.stZeroCtrl.iSleepTimeCnt++ >= (m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iBottomSleep*1000)/OPERATE_TRREAD_SLEEP_TIME)
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

void EDM_OP_HOLE::EdmHolePassCtl()
{
	float fStable;
	int iCnt=0;
	int iCalc =0;
	int iPulseSleep;
	int iStartStable;
	float fToff;
	int iToffIndex;
	int iOpLabelWorkPos;	

	m_pEdm->GetEdmMacPassPara(&m_stMacPassPara);

	if (!m_stMacPassPara.bOver)
		return;

    //TODOm_stOpCtrl.stZeroCtrl.stPassCtl.iVoltage = ReadVoltage();
    m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltage = 0;

	if (++m_stOpCtrl.stZeroCtrl.stPassCtl.iFiltCnt<5)
		return;


	iCalc = 0;
	for (int i=0;i<m_stOpCtrl.stZeroCtrl.stPassCtl.iSpeedIndex;i++)
	{
		iCalc += m_stOpCtrl.stZeroCtrl.stPassCtl.iSpeedStable[i];
	}
	iCalc =  iCalc/(m_stOpCtrl.stZeroCtrl.stPassCtl.iSpeedIndex+1);

	iCnt=0;
	for (int i=0;i<=m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltageRecIndex&& m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltageRecIndex>0;i++)
	{
		iCnt += m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltageRec[i];
	}
	iCnt = iCnt/(m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltageRecIndex+1);

	//m_dlgPassChart.SetRealTimePara(m_stMacComm.stMoveCtrlComm[m_stUserSystemSet.iOpLabel].iRelLabel ,iCnt,iCalc);	

	iOpLabelWorkPos = m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iMachPos 
		              - m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel].iWorkPosSet;

	m_stOpStatus.stCycle.stPassChart.bRealTimeIn = TRUE;
	m_stOpStatus.stCycle.stPassChart.iPosRealTime = iOpLabelWorkPos;
	m_stOpStatus.stCycle.stPassChart.iElecRealTime = iCnt;
	m_stOpStatus.stCycle.stPassChart.iSpeedRealTime = iCalc;

	if (m_stOpCtrl.stZeroCtrl.stPassCtl.enPassMode == PASS_NONE
		&& iOpLabelWorkPos>= m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[0].iOpLen+1000
		&& !m_stOpCtrl.stZeroCtrl.stPassCtl.bStop)
	{
		m_stOpCtrl.stZeroCtrl.stPassCtl.iSum += m_stMacPassPara.iFeedCnt;
		m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltageSum += m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltage;
		m_stOpCtrl.stZeroCtrl.stPassCtl.iStageCnt++;
	}

	if ( iOpLabelWorkPos>= m_stOpCtrl.stZeroCtrl.stPassCtl.iPosMax)
	{
		m_stOpCtrl.stZeroCtrl.stPassCtl.iPosMax = iOpLabelWorkPos;
	}

	if (m_stOpCtrl.stZeroCtrl.stPassCtl.enPassMode == PASS_NONE
		&& iOpLabelWorkPos>=m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[0].iOpLen+500
		&& !m_stOpCtrl.stZeroCtrl.stPassCtl.bEntryOver)
	{
		if (m_stMacPassPara.iBackCnt<20)
			m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedEntry++;
		else
			m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedEntry = 0;

		if (m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedEntry>=20 && iOpLabelWorkPos>=500
			|| iOpLabelWorkPos>=1500)
		{
			m_stOpCtrl.stZeroCtrl.stPassCtl.bEntryOver = TRUE;
			m_stOpCtrl.stZeroCtrl.stPassCtl.iSum = 0;
			m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltageSum = 0;
			m_stOpCtrl.stZeroCtrl.stPassCtl.iStageCnt = 0;
		}
	}

	iStartStable = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iOpLenAll/4;
	if (iStartStable>10000)
		iStartStable = 10000;

	m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStable[m_stOpCtrl.stZeroCtrl.stPassCtl.iStabelIndex] = m_stMacPassPara.iFeedCnt;
	m_stOpCtrl.stZeroCtrl.stPassCtl.iBackStable[m_stOpCtrl.stZeroCtrl.stPassCtl.iStabelIndex] = m_stMacPassPara.iBackCnt;
	m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltageStable[m_stOpCtrl.stZeroCtrl.stPassCtl.iStabelIndex] = m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltage;

	if (iOpLabelWorkPos>=m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iJudgePos)
	{
		/////////////////////
		iPulseSleep = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecPage[m_stOpStatus.stCycle.iOpPage].iToff;
		if (!m_stOpCtrl.stZeroCtrl.stPassCtl.bStop && m_stOpCtrl.stZeroCtrl.stPassCtl.bEntryOver && m_stOpCtrl.stZeroCtrl.stPassCtl.iStageCnt>1) 
		{
			m_stOpCtrl.stZeroCtrl.stPassCtl.bStop = TRUE;
            GetElecToff(iPulseSleep,&iToffIndex);
			iToffIndex += m_pEdm->m_stSysSet.stSetNoneLabel.iToff;
            fToff = GetElecToffByIndex(iToffIndex);
			iCalc = m_stOpCtrl.stZeroCtrl.stPassCtl.iSum/m_stOpCtrl.stZeroCtrl.stPassCtl.iStageCnt*0.7;
			m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltageStd = (m_stPassSet.fElec* (((float)iPulseSleep)/fToff)+0.17)
				*((float)m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltageSum/(float)m_stOpCtrl.stZeroCtrl.stPassCtl.iStageCnt);
			m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStd = iCalc;
			if (m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStd>35)
				m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStd=35;
			if (m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStd<15)
				m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStd=15;
		}


		if (m_stOpCtrl.stZeroCtrl.stPassCtl.enPassMode == PASS_NONE)
		{
			if (m_stOpCtrl.stZeroCtrl.stPassCtl.enPassMode == PASS_NONE)
			{
				iCnt = 0;
				for (int i=0;i<=m_stOpCtrl.stZeroCtrl.stPassCtl.iStabelIndex;i++)
				{
					if (m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStable[i]<=m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStd)
					{
						if(m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStable[i]+m_stOpCtrl.stZeroCtrl.stPassCtl.iBackStable[i]==0)
							fStable =  1;
						else
							fStable =  (float)m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStable[i]/
							(float)(m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStable[i]+m_stOpCtrl.stZeroCtrl.stPassCtl.iBackStable[i]);

						if (fStable<=0.75)
							iCnt++;					
					}
				}
				if (iCnt>=4 && m_stOpCtrl.stZeroCtrl.stPassCtl.bEntryOver)
				{
					m_stOpCtrl.stZeroCtrl.stPassCtl.enPassMode = PASS_LOW;
				}		

				iCnt = 0;
				for (int i=0;i<=m_stOpCtrl.stZeroCtrl.stPassCtl.iStabelIndex;i++)
				{
					if (m_stOpCtrl.stZeroCtrl.stPassCtl.iBackStable[i]>=16)
						iCnt++;				
				}
				if (iCnt>=4 && m_stOpCtrl.stZeroCtrl.stPassCtl.bEntryOver)
				{
					m_stOpCtrl.stZeroCtrl.stPassCtl.enPassMode = PASS_LOW;
				}


				iCnt = 0;
				if (m_stOpCtrl.stZeroCtrl.stPassCtl.iPosMax<=iOpLabelWorkPos)
				{
					for (int i=0;i<=m_stOpCtrl.stZeroCtrl.stPassCtl.iStabelIndex;i++)
					{
						if (m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltageStable[i]<=m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltageStd)
						{
							if(m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStable[i]+m_stOpCtrl.stZeroCtrl.stPassCtl.iBackStable[i]==0)
								fStable =  1;
							else
								fStable =  (float)m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStable[i]/
								(float)(m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStable[i]+m_stOpCtrl.stZeroCtrl.stPassCtl.iBackStable[i]);

							if (fStable>=0.9)
								iCnt++;
						}
					}
				}			
				if (iCnt==5 && m_stOpCtrl.stZeroCtrl.stPassCtl.bEntryOver)
				{
					m_stOpCtrl.stZeroCtrl.stPassCtl.enPassMode = PASS_LOW;
				}
			}		
		}
	}


	if (++m_stOpCtrl.stZeroCtrl.stPassCtl.iStabelIndex>=5)
	{
		m_stOpCtrl.stZeroCtrl.stPassCtl.iStabelIndex = 5;
		for (int i=0;i<m_stOpCtrl.stZeroCtrl.stPassCtl.iStabelIndex;i++)
		{
			m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStable[i]= m_stOpCtrl.stZeroCtrl.stPassCtl.iFeedStable[i+1];
			m_stOpCtrl.stZeroCtrl.stPassCtl.iBackStable[i]= m_stOpCtrl.stZeroCtrl.stPassCtl.iBackStable[i+1];
			m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltageStable[i] = m_stOpCtrl.stZeroCtrl.stPassCtl.iVoltageStable[i+1];
		}
	}
}

//电极修整
BOOL EDM_OP_HOLE::EdmHoleGo2AdjustPos()
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

BOOL EDM_OP_HOLE::EdmHoleReturnOpPos()
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

BOOL EDM_OP_HOLE::EdmHoleGo2StartPos()
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
BOOL EDM_OP_HOLE::EdmHoleResetStartPos()
{
	DIGIT_CMD cmd;
	QString strRec;
	QString strTmpRec;

	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
		return FALSE;

	m_stOpStatus.stCycle.iCycleIndex = 5;
    memset(&cmd,0,sizeof(DIGIT_CMD));
    m_pEdm->EdmSetProtect(false);

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
	memset(&m_stOpCtrl.stZeroCtrl,0,sizeof(HOLE_ZERO_CTRL));
	m_stOpStatus.stCycle.iTimeCnt = 0;
    m_stOpStatus.stCycle.iTimeSec = 0;

	if (m_stOpStatus.enOpType!=OP_HOLE_SING)
	{
		m_stOpStatus.iCmdIndex++;
		if (m_stOpStatus.enOpType==OP_HOLE_MILL)
		{
			m_stOpStatus.iCmdIndex = 0;
			m_stOpStatus.bOpOver = TRUE;
		}
		m_pOpFile->SetEdmElecIndex(m_stOpStatus.iCmdIndex+1);
		m_pOpFile->GetLastElecName(m_stOpStatus.iCmdIndex);
	}

	if (m_stOpStatus.enOpType==OP_HOLE_SING || 
		 (m_stOpStatus.enOpType!=OP_HOLE_SING
		   &&!m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.bContinueOp)
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

	if (m_enOpType == OP_TYPE_NONE || EDM_OP::m_bInOp==FALSE)
	{
		return;
	}

	if (m_pEdm->m_stEdmComm.enMvStatus != RULE_MOVE_OVER)
	{
		while (!m_pEdm->EdmStopMove(false))
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
		cmd.stAxisDigit[cmd.iAxisCnt].iDistance = m_pEdm->m_stEdmComm.stMoveCtrlComm[i].iMachPos 
			- m_iWorkPos_All[(int)m_pOpFile->m_enCoor][i];
		cmd.iAxisCnt++;
	}

	while(!m_pEdm->EdmSendMovePara(&cmd))
	{
	}

	m_pEdm->ReSetWorkPosSetByIndex((int)m_pOpFile->m_enCoor,&(m_iWorkPos_All[(int)m_pOpFile->m_enCoor][0]));	

	m_pEdm->EdmSetProtect(true);
}


void EDM_OP_HOLE::SetEdmHolePower(BOOL bPower,BOOL bPrune,BOOL bOtherClose)
{
	BOOL bRotate = TRUE;
	BOOL bLowPump = FALSE;
	BOOL bOther = TRUE;

	EDM_OP::m_bSetPower = bPower;	

	if (bPower)
	{
		bRotate = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.bRotateValidate;
		bLowPump = m_pEdm->m_stSysSet.stSetNoneLabel.bLowPumpUnion;
		bOther = TRUE;
	}
	else
	{
		bPrune = FALSE;
		if (bOtherClose)
		{
			bOther = FALSE;
			bRotate = FALSE;
			bLowPump = FALSE;
		}
	}

	m_pEdm->EdmPower(bPower);
	m_pEdm->EdmPrune(bPrune);	

	if (bPower || bOtherClose)
	{
		m_pEdm->EdmHighPump(bOther);
		m_pEdm->EdmLowPump(bLowPump);
		m_pEdm->EdmRotate(true,bRotate,-1,false);	
	}		
}


BOOL EDM_OP_HOLE::ExteedTimeAlarm()
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

	iCntMax = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iTimeMax;
	iCntMin = m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iTimeMin;
    if (m_it==m_ListStage.end()  && m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iTimeMin>0 && (iCntMin>m_stOpStatus.stCycle.iTimeSec
        || iCntMax>iCntMin&& iCntMax<m_stOpStatus.stCycle.iTimeSec))
	{
		m_stOpStatus.enErrAll.errOp = OP_EXCEED_TIME;
		return TRUE;
	}

	return FALSE;
}

BOOL EDM_OP_HOLE::PoleLenAlarm()
{
	int iLabel = m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel;

	if (m_pEdm->m_stSysSet.stSetNoneLabel.bOpDir)
	{
		if (m_pEdm->m_stEdmComm.stMoveCtrlComm[iLabel].iMachPos+m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iOpLenAll
			> m_pEdm->m_stEdmKpInt.stAxisCtrlKp[iLabel].stSoftPara.iTopPos)
		{
			m_stOpStatus.enErrAll.errOp = OP_LEN_POLE;
			return TRUE;
		}
	}
	else
	{
		if (m_pEdm->m_stEdmComm.stMoveCtrlComm[iLabel].iMachPos-m_pOpFile->m_mpElecMan[m_pOpFile->m_strElec].stElecOral.iOpLenAll
			< m_pEdm->m_stEdmKpInt.stAxisCtrlKp[iLabel].stSoftPara.iLowerPos)
		{
			m_stOpStatus.enErrAll.errOp = OP_LEN_POLE;
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
		if (!m_pOpFile->IsPauseCmd(str) 
			&& !m_pOpFile->IsStrElecCmd(str) 
			&& !m_pOpFile->IsMillFile(str) 
			&& !m_pOpFile->IsOverCmd(str))
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



BOOL EDM_OP_HOLE::EdmOpMvAheadChkLabel()
{
	DIGIT_CMD cmd;
	CHECK_MODE enMode =CHECK_ELASTIC;

	if (m_stOpCtrl.stZeroCtrl.bStageLast)
	{
		m_stOpCtrl.stZeroCtrl.bStageLast = FALSE;
		return TRUE;
	}

	m_pEdm->EdmStopMove(false);

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
        //TODOm_stOpCtrl.stZeroCtrl.iCheckPos = m_pEdm->GetNLabelMac();
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



BOOL EDM_OP_HOLE::EdmOpMvWaitChkLabel()
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
        //TODO m_stOpCtrl.stZeroCtrl.iCheckPos = m_pEdm->GetNLabelMac();
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


BOOL EDM_OP_HOLE::EdmOpMvBackChkLabel()
{
	DIGIT_CMD cmd;
	CHECK_MODE enMode =CHECK_ELASTIC;

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
