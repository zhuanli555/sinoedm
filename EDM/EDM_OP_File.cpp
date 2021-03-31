//edm_op_file.cpp edm_op_file实现文件
#include "EDM_OP_File.h"
#include <math.h>

EDM_OP_File::EDM_OP_File()
{
	m_strElec = EDM::m_strElecDefault;
	m_sFile = "";
	m_pEdm =  EDM::GetEdmInstance();

	m_enAim = AIM_G90;
	m_enOrbit = ORBIT_G00;
	m_enCoor = (EDM_COOR_TYPE)m_pEdm->m_iWorkIndex;
	m_enOpFileErr = OP_FILE_NO_ERR;
	m_enOpType = OP_TYPE_NONE;

	m_iCmdNum = 0;
}

EDM_OP_File::~EDM_OP_File()
{	
    m_pEdm = NULL;
}


//要修改
unsigned char EDM_OP_File::SetEdmOpFile(QString sPath,QString sFile)
{
	vector<QString>::iterator it;
	int iHoleIndexBak;

	m_iCmdNum = 0;
	m_enOpFileErr = OP_FILE_NO_ERR;

    if (CmdHandle::ReadCmdFromFile(sPath,sFile,m_vCmdStd,&m_mpElecMan))
    {
		it=m_vCmdStd.begin();
		if (it==m_vCmdStd.end())
		{
			m_enOpFileErr = OP_FILE_NO_CMD;
			return FALSE;
		}
		while (it!=m_vCmdStd.end())
		{
			m_iCmdNum++;
			it++;
		}
        iHoleIndexBak = m_mpElecMan[sFile].stElecOral.iOpHoleIndex;
		SetEdmElecIndex(iHoleIndexBak);

		m_sFile = sFile;
        m_sPath = sPath;
		return TRUE;
	}
	else
	{
		m_enOpFileErr= OP_FILE_NO_EXIST;
	}

	return FALSE;
}

//要修改
void EDM_OP_File::SetEdmOpElec(QString str,MAC_ELEC_PARA elec,unsigned char bStart,unsigned char bCycleStart,int iPageIndex)
{
    int iHoleIndexBak = 0;
	unsigned char bWrite = FALSE;

    iHoleIndexBak = m_mpElecMan[str].stElecOral.iOpHoleIndex;
    memcpy(&m_mpElecMan[str],&elec,sizeof(MAC_ELEC_PARA));

	if (bStart)
    {
        m_mpElecMan[str].stElecOral.iOpHoleIndex = iHoleIndexBak;
        if (m_strElec==str && iPageIndex==elec.iParaIndex)
		{
			bWrite = TRUE;
		}
	}
	else
	{
        INFO_PRINT();
		if (bCycleStart)
		{
			elec.stElecOral.iOpHoleIndex = iHoleIndexBak;
            if (m_strElec==str && iPageIndex==elec.iParaIndex)
			{
				bWrite = TRUE;
			}
		}

		SetEdmElecIndex(elec.stElecOral.iOpHoleIndex);
	}

    if (bWrite)
    {
        if(m_pEdm->WriteElecPara(&elec.stElecPage[elec.iParaIndex],"EDM_OP_File::SetEdmOpElec") ==-1)
            m_enOpFileErr=OP_FILE_ERR_ELEC;
    }
}


void EDM_OP_File::SetEdmElecIndex(int iCmdIndex)
{
	iCmdIndex = max(iCmdIndex,1);
	iCmdIndex = min(iCmdIndex,m_iCmdNum);
    m_mpElecMan[m_sFile].stElecOral.iOpHoleIndex = iCmdIndex;
    m_mpElecMan[m_sFile].stElecOral.iOpHoleAll = m_iCmdNum;
}

unsigned char EDM_OP_File::IsPauseCmd(QString str)
{
    str = str.toUpper().trimmed();
	if (str=="M00")
		return TRUE;

	return FALSE;
}

unsigned char EDM_OP_File::IsMillStart(QString str)
{
    str = str.toUpper().trimmed();
	if (str=="(")
		return TRUE;

	return FALSE;
}

unsigned char EDM_OP_File::IsMillOver(QString str)
{
    str = str.toUpper().trimmed();
	if (str==")")
		return TRUE;

	return FALSE;
}


unsigned char EDM_OP_File::IsOverCmd(QString str)
{
    str = str.toUpper().trimmed();
	if (str=="M02")
		return TRUE;

	return FALSE;
}

void EDM_OP_File::SetOpType(MAC_OPERATE_TYPE enOpType)
{
	m_enOpType = enOpType;
}

void EDM_OP_File::PlusDigit2Cmd()
{
	DIGIT_CMD cmd;
	DIGIT_CMD cmdDefault;
	DIGIT_CMD cmd_Loc;
	int iCmdIndex=0;
	QString str;
	QString str_Loc;
    int iSum[MAC_LABEL_COUNT];
	int iSumLastMac[MAC_LABEL_COUNT];
	int iWorkPos_All[6][MAC_LABEL_COUNT];
	int iLabel;
	unsigned char bFirstEntry = TRUE;
    CmdHandle* pCmdHandle;

	m_enAim = AIM_G90;
	m_enOrbit = ORBIT_G01;

	for (int i=0;i<6;i++)
	{
		m_pEdm->GetWorkPosSetByIndex(i,&(iWorkPos_All[i][0]));
	}

	m_vCmd.clear();
	m_vCmdLoc.clear();
    for(iCmdIndex = 0;iCmdIndex<m_vCmdStd.size();iCmdIndex++)
    {
        str = m_vCmdStd[iCmdIndex];
        str_Loc = m_vCmdStd[iCmdIndex];
        if (IsPauseCmd(str) || IsOverCmd(str))
        {
        }
        else
        {
            memset(&cmdDefault,0,sizeof(DIGIT_CMD));
            cmdDefault.enAim = m_enAim;
            cmdDefault.enOrbit = m_enOrbit;
            cmdDefault.enCoor = m_enCoor;
            cmdDefault.iFreq = MAC_INT_FREQ;
            pCmdHandle = new CmdHandle(FALSE,str,&cmd,&cmdDefault);
            delete pCmdHandle;
            if (m_enAim != cmd.enAim)
                m_enAim = cmd.enAim;
            if (m_enOrbit != cmd.enOrbit)
                m_enOrbit = cmd.enOrbit;

            //初始点位
            if (bFirstEntry)
            {
                bFirstEntry = FALSE;
                m_enCoor = cmd.enCoor;

                if (cmd.enAim == AIM_G91)
                {
                    m_enOpFileErr = OP_FILE_EDIT_ERR;
                }

                for (int k=0;k<MAC_LABEL_COUNT;k++)
                {
                    iSum[k] = m_pEdm->m_stEdmComm.stMoveCtrlComm[k].iMachPos  - iWorkPos_All[(int)cmd.enCoor][k];
                    if (m_pEdm->m_stEdmKpInt.stAxisCtrlKp[k].stSoftPara.bRotateLabel)
                    {
                        iSum[k] %= 360000;
                        if (iSum[k] <0)
                           iSum[k] += 360000;
                    }
                    iSumLastMac[k] = m_pEdm->m_stEdmComm.stMoveCtrlComm[k].iMachPos;
                }
            }

            //坐标体系改变
            if (m_enCoor!=cmd.enCoor)
            {
                m_enOpFileErr = OP_FILE_EDIT_ERR;
            }

            //命令起始点
            CalcStartPt(&cmd_Loc,iSumLastMac);

            //计算点位
            for (int k=0;k<cmd.iAxisCnt;k++)
            {
                iLabel = cmd.stAxisDigit[k].iLabel;
                if (cmd.enAim==AIM_G92)
                {
                    m_enOpFileErr = OP_FILE_EDIT_ERR;
                }
                else if (cmd.enAim == AIM_G90)
                {
                    iSumLastMac[iLabel] = iSumLastMac[iLabel] + cmd.stAxisDigit[k].iDistance - iSum[iLabel];
                    iSum[iLabel] = cmd.stAxisDigit[k].iDistance;
                }
                else if (cmd.enAim == AIM_G91)
                {
                   iSum[iLabel] += cmd.stAxisDigit[k].iDistance;
                   iSumLastMac[iLabel] += cmd.stAxisDigit[k].iDistance;
                }

                if (m_pEdm->m_stEdmKpInt.stAxisCtrlKp[iLabel].stSoftPara.bRotateLabel)
                {
                    iSum[iLabel] %= 360000;
                    if (iSum[iLabel] <0)
                        iSum[iLabel] += 360000;
                }
                cmd.stAxisDigit[k].iDistance = iSum[iLabel];
            }

            cmd.enAim = AIM_G90;
            cmd.enCoor = m_enCoor;
            CmdHandle::DigitCmd2QString(&cmd,str);
            CmdHandle::DigitCmd2QString(&cmd_Loc,str_Loc);
        }
        m_vCmd.push_back(str);
        if (IsOverCmd(str))
        {
            CalcStartPt(&cmd_Loc,iSumLastMac);
            CmdHandle::DigitCmd2QString(&cmd_Loc,str_Loc);
        }
        m_vCmdLoc.push_back(str_Loc);
    }

}

void EDM_OP_File::CalcStartPt(DIGIT_CMD* pCmd,int iSum[])
{

	//命令起始点
	memset(pCmd,0,sizeof(DIGIT_CMD));
	pCmd->enAim = AIM_G90;
	pCmd->enCoor = m_enCoor;
	pCmd->enOrbit = ORBIT_G01;
	pCmd->iFreq = MAC_INT_FREQ;
	for (int k=0;k<MAC_LABEL_COUNT;k++)
	{
		if (m_pEdm->m_stEdmKpInt.stAxisCtrlKp[k].stSoftPara.bRotateLabel)
		{
			iSum[k] %= 360000;
			if (iSum[k] <0)
				iSum[k] += 360000;
		}

		if (k==m_pEdm->m_stSysSet.stSetNoneLabel.iOpLabel)
		{
			continue;
		}
		pCmd->stAxisDigit[pCmd->iAxisCnt].iLabel = k;
		pCmd->stAxisDigit[pCmd->iAxisCnt].iDistance = iSum[k];
		pCmd->iAxisCnt++;
	}
}

