//edm_op.cpp edm_op实现文件
#include "EDM_OP.h"
#include <math.h>

unsigned char EDM_OP::m_bSetPower = FALSE;
unsigned char EDM_OP::m_bStartCount = FALSE;
unsigned char EDM_OP::m_bInOp = FALSE;

EDM_OP::EDM_OP()
{
	m_pEdm =  EDM::GetEdmInstance();
	memset(&m_stOpStatus,0,sizeof(OP_STATUS));
	m_pEdmOpPre = NULL;
	m_bCalc = FALSE; 
	m_pOpFile = NULL;
	m_enOpType = OP_TYPE_NONE;

	if (m_pEdm->m_stSysSet.iAxistLabel>=0 && m_pEdm->m_stSysSet.iAxistLabel<MAC_LABEL_COUNT)
	{
		m_iSafeLabelMacPos = m_pEdm->m_stEdmComm.stMoveCtrlComm[m_pEdm->m_stSysSet.iAxistLabel].iMachPos;
    }
    for(int i = 0;i<MAC_LABEL_COUNT;i++)
    {
        for(int j = 0;j<6;j++)
        {
            m_iWorkPos_All[i][j] = m_pEdm->m_iCoor[i][j];
        }
    }
    m_iWholeFreq = CmdHandle::GetSpeedFreq(m_pEdm->m_stSysSet.stSetNoneLabel.iWholeSpeed);
}

EDM_OP::~EDM_OP()
{
	if (m_pOpFile)
	{
        delete m_pOpFile;
    }
}

unsigned char EDM_OP::EdmOpErr()
{
	if (m_pEdm->m_stEdmComm.enMvTrouble != MOVE_NO_TROUBLE 
        || m_stOpStatus.errOp != OP_NO_ERR
        || m_stOpStatus.errFile != OP_FILE_NO_ERR
		|| m_pEdm->m_stEdmShowData.stStatus.bAxisOffset)
	{
        m_stOpStatus.errOp = OP_HARD_ERR;
		m_stOpStatus.stCycle.bPauseCmd = TRUE;
		m_stOpStatus.bStart = FALSE;		
		return TRUE;
	}
	return FALSE;
}


void EDM_OP::SetEdmOpFile(QString sPath,QString sFile)
{
	if (m_pOpFile)
	{
		delete m_pOpFile;
		m_pOpFile = NULL;
	}

	m_pOpFile = new EDM_OP_File();
	if (m_pOpFile)
	{
		m_sPath = sPath;
		m_pOpFile->SetOpType(m_enOpType);
		m_pOpFile->SetEdmOpFile(sPath,sFile);
	}	
}

void EDM_OP::CalcDigitCmd()
{
	if (m_bCalc)
	{
		return;
	}

	m_bCalc = TRUE;
    for(int i = 0;i<MAC_LABEL_COUNT;i++)
    {
        for(int j = 0;j<6;j++)
        {
            m_iWorkPos_All[i][j] = m_pEdm->m_iCoor[i][j];
        }
    }
	if (m_pOpFile)
	{
		m_pOpFile->PlusDigit2Cmd();
	}
}

void EDM_OP::SetEdmOpElec(QString str,MAC_ELEC_PARA elec)
{
	if (m_pOpFile)
    {
        m_pEdm->SaveElecElem(str,&elec);
		if (m_stOpStatus.bStart)
		{
			EdmOpStageRestart();

		}
	}
}
