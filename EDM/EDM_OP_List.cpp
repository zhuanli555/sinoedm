//edm_op.cpp edm_opʵ���ļ�
#include "EDM_OP_List.h"

EDM_OP_List* EDM_OP_List::m_pEdmOpList = NULL;

EDM_OP_List::EDM_OP_List(QObject *parent):QObject(parent)
{
	m_bOver = FALSE;
	m_enOpType = OP_TYPE_NONE;
	m_pEdmOp = NULL;
	m_pEdmOp = new EDM_OP_HOLE(OP_TYPE_NONE);
	m_bChange = FALSE;
}


EDM_OP_List::~EDM_OP_List()
{
	DeleteEdmOp();
}

void EDM_OP_List::SetEdmOpType(MAC_OPERATE_TYPE enType)
{
	m_bOver = FALSE;
	m_enOpType = enType;
	m_bChange = TRUE;
	if (m_pEdmOp)
	{
		delete m_pEdmOp;
		m_pEdmOp = NULL;
	}

	m_pEdmOp = new EDM_OP_HOLE(enType);
	EDM_OP::m_bSetPower = FALSE;
	EDM_OP::m_bInOp = FALSE;
	EDM_OP::m_bStartCount = FALSE;
}

EDM_OP_List* EDM_OP_List::GetEdmOpListPtr()
{
	if (!m_pEdmOpList)
	{
		m_pEdmOpList = new EDM_OP_List();
	}

	return m_pEdmOpList;
}

void EDM_OP_List::DeleteEdmOpList()
{
	delete m_pEdmOpList;
	m_pEdmOpList = NULL;
}

void EDM_OP_List::ResetEdmOpFile()
{
	SetEdmOpFile(m_sPath,m_sFile);
}

void EDM_OP_List::DeleteEdmOp()
{
	EDM_OP* pOpPre;
	EDM_OP* pOpTmp;

	if (m_pEdmOp)
	{
		pOpPre = m_pEdmOp->m_pEdmOpPre;
		while (pOpPre)
		{
			pOpTmp = pOpPre->m_pEdmOpPre;
			delete pOpPre;
			pOpPre = pOpTmp;
		}
		delete m_pEdmOp;
		m_pEdmOp = NULL;		
	}	
}

unsigned char EDM_OP_List::SetEdmOpFile(QString sPath,QString sFile)
{	
	m_bChange = TRUE;	
	if (m_pEdmOp)
	{		
		if (EDM_OP::m_bInOp)
		{
			return FALSE;
		}
		m_sPath = sPath;
		m_sFile = sFile;
		m_pEdmOp->SetEdmOpFile(sPath,sFile);
		return TRUE;
	}

	return FALSE;
}

void EDM_OP_List::SetStart(bool bStart)
{
	if (bStart)
	{
		if (m_bOver)
		{
			DeleteEdmOp();
			SetEdmOpType(m_enOpType);
			m_pEdmOp->SetEdmOpFile(m_sPath,m_sFile);	
			m_bOver = FALSE;
		}
	}

	if (m_pEdmOp)
	{
		m_pEdmOp->EdmOpSetStart(bStart);
	}
}

void EDM_OP_List::CarryOn()
{
	EDM_OP* pOp;

	if (m_enOpType==OP_TYPE_NONE || !m_pEdmOp)
	{
		return;
	}

	if (m_pEdmOp->m_stOpStatus.bNewOp)
	{
		m_pEdmOp->m_stOpStatus.bNewOp = FALSE;
		if (m_pEdmOp->m_stOpStatus.pNewHoleOp)
		{
			pOp = m_pEdmOp;
			m_pEdmOp = (EDM_OP*)(m_pEdmOp->m_stOpStatus.pNewHoleOp);
			m_pEdmOp->m_pEdmOpPre = pOp;
			m_pEdmOp->CalcDigitCmd();
			m_pEdmOp->m_stOpStatus.bStart = TRUE;
			pOp->m_stOpStatus.pNewHoleOp = NULL;
		}
	}

	if (m_pEdmOp->m_stOpStatus.bOpOver)
	{
		pOp = m_pEdmOp;
		if (m_pEdmOp->m_pEdmOpPre)
		{
			m_pEdmOp = m_pEdmOp->m_pEdmOpPre;
			delete pOp;
		}
		else
		{				
			delete pOp;
			m_pEdmOp = NULL;
			SetEdmOpType(OP_TYPE_NONE);
			m_pEdmOp->SetEdmOpFile(m_sPath,m_sFile);
			m_bOver = TRUE;		
			return;
		}		
	}

	if (!m_bOver)
	{
		if (m_pEdmOp)
		{
			m_pEdmOp->EdmOpCarry();
		}
		else
		{
			m_bOver = TRUE;
		}
	}
}


unsigned char EDM_OP_List::GetOpFileInfo(QString* pStringFile,MAP_ELEC_MAN* pElec,vector<QString>* pCmd,vector<QString>* pCmdAbs)
{
	MAP_ELEC_MAN elecMap;
	map<QString,MAC_ELEC_PARA>::iterator it = pElec->begin();
	vector<QString>::iterator itVect;
	QString str;
	MAC_ELEC_PARA elec;
	int i=0;

	pCmd->clear();
	pCmdAbs->clear();

	it = pElec->begin();
	while(it!=pElec->end())
		pElec->erase(it++);

	*pStringFile = "";

	if ( !m_pEdmOp->m_pOpFile || !m_pEdmOp)
	{
		return FALSE;
	}

	if (m_pEdmOp->m_pOpFile->m_enOpFileErr)
	{
		return FALSE;
	}

	*pStringFile = m_pEdmOp->m_pOpFile->m_sFile;

	it = m_pEdmOp->m_pOpFile->m_mpElecMan.begin();
	while (it != m_pEdmOp->m_pOpFile->m_mpElecMan.end())
	{
		str = it->first;
		memcpy(&elec,&(it->second),sizeof(MAC_ELEC_PARA));
		pElec->insert(make_pair(str,elec));
		it++;
	}

	itVect = m_pEdmOp->m_pOpFile->m_vCmdStd.begin();
	i=0;
	while (itVect != m_pEdmOp->m_pOpFile->m_vCmdStd.end())
	{
		pCmd->push_back(m_pEdmOp->m_pOpFile->m_vCmdStd[i]);
		itVect++;
		i++;
	}

	itVect = m_pEdmOp->m_pOpFile->m_vCmd.begin();
	i=0;
	while (itVect != m_pEdmOp->m_pOpFile->m_vCmd.end())
	{
		pCmdAbs->push_back(m_pEdmOp->m_pOpFile->m_vCmd[i]);
		itVect++;
		i++;
	}

	return TRUE;
}

void EDM_OP_List::GetOpStatus(OP_STATUS* pStatus)
{
	return;
}

void EDM_OP_List::EdmOpListOver()
{
	if (m_pEdmOp)
	{
		m_pEdmOp->EdmOpOver();
	}
	DeleteEdmOp();
	SetEdmOpType(OP_TYPE_NONE);
	m_pEdmOp->SetEdmOpFile(m_sPath,m_sFile);
}
