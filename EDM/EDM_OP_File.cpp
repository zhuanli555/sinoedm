//edm_op_file.cpp edm_op_file实现文件
#include "EDM_OP_File.h"
#include <math.h>

EDM_OP_File::EDM_OP_File()
{
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

unsigned char EDM_OP_File::ReadCmdFromFile(QString strPath,QString strFile)
{
    QString	  strFullName =  strPath + "/" + strFile;
    QFile inFile(strFullName);
    QString str;

    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
              return 0;

    m_vCmdStd.clear();
    QTextStream in(&inFile);
    while(!in.atEnd())
    {
        str = in.readLine();
        if (!str.isEmpty())
        {
            str = str.trimmed().toUpper();
            m_vCmdStd.push_back(str);
        }
    }

    inFile.close();

    return 1;
}
//要修改
unsigned char EDM_OP_File::SetEdmOpFile(QString sPath,QString sFile)
{
    vector<QString>::iterator it;

	m_iCmdNum = 0;
	m_enOpFileErr = OP_FILE_NO_ERR;

    if (ReadCmdFromFile(sPath,sFile))
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
	int iCmdIndex=0;
    QString str;
    int iSum[MAC_LABEL_COUNT];
	int iSumLastMac[MAC_LABEL_COUNT];
	int iWorkPos_All[MAC_LABEL_COUNT][6];
	int iLabel;
	unsigned char bFirstEntry = TRUE;
    CmdHandle* pCmdHandle;

	m_enAim = AIM_G90;
	m_enOrbit = ORBIT_G01;
    for(int i = 0;i<MAC_LABEL_COUNT;i++)
    {
        for(int j = 0;j<6;j++)
        {
            iWorkPos_All[i][j] = m_pEdm->m_iCoor[i][j];
        }
    }
    m_vCmd.clear();
    for(iCmdIndex = 0;iCmdIndex<m_vCmdStd.size();iCmdIndex++)
    {
        str = m_vCmdStd[iCmdIndex];
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
                    iSum[k] = m_pEdm->m_stEdmComm.stMoveCtrlComm[k].iMachPos  - iWorkPos_All[k][(int)cmd.enCoor];
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
        }
        m_vCmd.push_back(str);
    }

}
