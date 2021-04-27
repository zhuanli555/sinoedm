//edm_op.cpp edm_op实现文件
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

void EDM_OP_List::CarryOnBefore(MAC_OPERATE_TYPE enType)
{
	DeleteEdmOp();
	SetEdmOpType(enType);
    m_pEdmOp->SetEdmOpFile(m_sPath,m_sFile);
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
