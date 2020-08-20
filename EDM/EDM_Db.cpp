//EDM_Db.cpp EDM_Db实现文件

#include "EDM_Db.h"

void EDM_Db::GetAxisLuoBuData(char cSymbol, int iVal[], int nCount)
{
#ifdef SQL	
	QString str;
	str = QString("SELECT * FROM %c_label order by iIndex").arg(cSymbol);
	q.exec(str);
	int j = 0;
	while (q.next() && j < nCount)
	{
		iVal[j] = q.value(0).toInt();
		j++;
	}
#else
	int i = 0;
	for(i = 0;i<nCount;i++)
	{
		if(i%5==0)
		{
			iVal[i] = 1;
		}
		else if(i%11 == 0)
		{
			iVal[i] = -1;
		}
		else if(i%12 == 0)
		{
			iVal[i] = -2;
		}
		else
		{
			iVal[i] = 0;
		}
	}
#endif
}

void EDM_Db::GetEdmCommPara(MAC_COMMON *pComm, int iWorkIndex)
{
#ifdef SQL
	int i = 0;
	QString str;
	QString strTmp;
	str = "iRelSet_" + QString("%d").arg(iWorkIndex);
	strTmp = QString("select iMachPos,bDir,%s from label_var").arg(str);
	q.exec(strTmp);
	while (q.next())
	{
		pComm->stMoveCtrlComm[i].iMachPos = q.value(0).toInt();
		pComm->stMoveCtrlComm[i].bDirMove = q.value(1).toInt();
		pComm->stMoveCtrlComm[i].iWorkPosSet = q.value(2).toInt();
		i++;
	}
#else
	pComm->stMoveCtrlComm[0].iMachPos = 0;
	pComm->stMoveCtrlComm[0].bDirMove = 1;
	pComm->stMoveCtrlComm[0].iWorkPosSet = 0;
#endif
}

void EDM_Db::SaveEdmCommPara(MAC_COMMON *pComm, int iWorkIndex, int a[][6])
{
	//	int i=0;
	//   QString str;
	//   QString strTmp;

	//	str.Format("%d",iWorkIndex);
	//	strTmp = "iRelSet_"+str;

	//	m_pTableLabelVar->MoveFirst();

	//	for (i=0;i<MAC_LABEL_COUNT && !m_pTableLabelVar->IsEof();i++)
	//	{
	//		m_pTableLabelVar->SetFieldValue("iMachPos",pComm->stMoveCtrlComm[i].iMachPos);
	//		m_pTableLabelVar->SetFieldValue("bDir",pComm->stMoveCtrlComm[i].bDirMove);
	//		m_pTableLabelVar->SetFieldValue(strTmp,a[i][iWorkIndex]);
	//		m_pTableLabelVar->Update();
	//		m_pTableLabelVar->MoveNext();
	//	}
}

void EDM_Db::SaveEdmWorkSet(int iWorkIndex, int a[][6])
{
	//	int i=0;
	//   QString str;
	//   QString strTmp;

	//	str.Format("%d",iWorkIndex);
	//	strTmp = "iRelSet_"+str;

	//	m_pTableLabelVar->MoveFirst();

	//	for (i=0;i<MAC_LABEL_COUNT && !m_pTableLabelVar->IsEof();i++)
	//	{
	//		m_pTableLabelVar->SetFieldValue(strTmp,a[i][iWorkIndex]);
	//		m_pTableLabelVar->Update();
	//		m_pTableLabelVar->MoveNext();
	//	}
}

void EDM_Db::GetEdmKpIntPara(MAC_KPINT *pInt, MAC_SYSTEM_SET *pSet)
{
#ifdef SQL
	int i = 0;
	QString strTmp;
	strTmp = QString("select cSymbol,bUse,bRotateLabel,iChiBu,bPosMonitor,fRasterLinear,iPosPrecision,bLimitNoUse,iZeroMakeUp,\
iLowerPos,iTopPos,iLocateArea,bDirectMotor from label");
	q.exec(strTmp);
	while (q.next())
	{
        pInt->stAxisCtrlKp[i].stSoftPara.cSymbol = q.value(0).toString().at(0).unicode();
		pInt->stAxisCtrlKp[i].stSoftPara.bUse = q.value(1).toInt();
		pInt->stAxisCtrlKp[i].stChiBuVal.iChiBu = q.value(2).toInt();
		pInt->stAxisCtrlKp[i].stSoftPara.bPosMonitor = q.value(3).toInt();
		pInt->stAxisCtrlKp[i].stSoftPara.iRasterLinear = q.value(4).toInt();
		pInt->stAxisCtrlKp[i].stSoftPara.iPosPrecision = q.value(4).toInt();
		pInt->stAxisCtrlKp[i].stSoftPara.bLimitNoUse = q.value(4).toInt();
		pInt->stAxisCtrlKp[i].stSoftPara.iZeroMakeUp = q.value(4).toInt();
		pInt->stAxisCtrlKp[i].stSoftPara.iLowerPos = q.value(4).toInt();
		pInt->stAxisCtrlKp[i].stSoftPara.iTopPos = q.value(4).toInt();
		pInt->stAxisCtrlKp[i].stSoftPara.iLocateArea = q.value(4).toInt();
		pInt->stAxisCtrlKp[i].stSoftPara.bDirectMotor = q.value(4).toInt();

		pSet->iSetChiBu[i] = pInt->stAxisCtrlKp[i].stChiBuVal.iChiBu;
		pSet->bSetUse[i] = pInt->stAxisCtrlKp[i].stSoftPara.bUse;
		pSet->bSetLimitNoUse[i] = pInt->stAxisCtrlKp[i].stSoftPara.bLimitNoUse;
		pSet->iSetZeroMakeUp[i] = pInt->stAxisCtrlKp[i].stSoftPara.iZeroMakeUp;
		pSet->bPosMonitor[i] = pInt->stAxisCtrlKp[i].stSoftPara.bPosMonitor;
		pSet->iRasterLinear[i] = pInt->stAxisCtrlKp[i].stSoftPara.iRasterLinear;
		pSet->iPosPrecision[i] = pInt->stAxisCtrlKp[i].stSoftPara.iPosPrecision;
		i++;
	}
#else
	pInt->stAxisCtrlKp[0].stSoftPara.cSymbol = 'x';
	pInt->stAxisCtrlKp[0].stSoftPara.bUse = 1;
	pInt->stAxisCtrlKp[0].stChiBuVal.iChiBu = 2;
	pInt->stAxisCtrlKp[0].stSoftPara.bPosMonitor = 0;
	pInt->stAxisCtrlKp[0].stSoftPara.iRasterLinear = 0;
	pInt->stAxisCtrlKp[0].stSoftPara.iPosPrecision = 0;
	pInt->stAxisCtrlKp[0].stSoftPara.bLimitNoUse = 0;
	pInt->stAxisCtrlKp[0].stSoftPara.iZeroMakeUp = 0;
	pInt->stAxisCtrlKp[0].stSoftPara.iLowerPos = 0;
	pInt->stAxisCtrlKp[0].stSoftPara.iTopPos = 400000;
	pInt->stAxisCtrlKp[0].stSoftPara.iLocateArea = 0;
	pInt->stAxisCtrlKp[0].stSoftPara.bDirectMotor = 0;

	pSet->iSetChiBu[0] = pInt->stAxisCtrlKp[0].stChiBuVal.iChiBu;
	pSet->bSetUse[0] = pInt->stAxisCtrlKp[0].stSoftPara.bUse;
	pSet->bSetLimitNoUse[0] = pInt->stAxisCtrlKp[0].stSoftPara.bLimitNoUse;
	pSet->iSetZeroMakeUp[0] = pInt->stAxisCtrlKp[0].stSoftPara.iZeroMakeUp;
	pSet->bPosMonitor[0] = pInt->stAxisCtrlKp[0].stSoftPara.bPosMonitor;
	pSet->iRasterLinear[0] = pInt->stAxisCtrlKp[0].stSoftPara.iRasterLinear;
	pSet->iPosPrecision[0] = pInt->stAxisCtrlKp[0].stSoftPara.iPosPrecision;
#endif
}

void EDM_Db::SaveEdmKpIntPara(MAC_KPINT *pInt, MAC_SYSTEM_SET *pSet)
{
	//	int i=0;
	//   QString strTmp;

	//	m_pTableLabel->MoveFirst();
	//	for (i=0;i<MAC_LABEL_COUNT && !m_pTableLabel->IsEof();i++)
	//	{
	//		pInt->stAxisCtrlKp[i].stChiBuVal.iChiBu = pSet->iSetChiBu[i];
	//		pInt->stAxisCtrlKp[i].stSoftPara.bUse = pSet->bSetUse[i];
	//		pInt->stAxisCtrlKp[i].stSoftPara.bLimitNoUse = pSet->bSetLimitNoUse[i];
	//		pInt->stAxisCtrlKp[i].stSoftPara.iZeroMakeUp = pSet->iSetZeroMakeUp[i];
	//		pInt->stAxisCtrlKp[i].stSoftPara.bPosMonitor = pSet->bPosMonitor[i];
	//		pInt->stAxisCtrlKp[i].stSoftPara.iRasterLinear = pSet->iRasterLinear[i];
	//		pInt->stAxisCtrlKp[i].stSoftPara.iPosPrecision = pSet->iPosPrecision[i];

	//		m_pTableLabel->SetFieldValue("bUse",pInt->stAxisCtrlKp[i].stSoftPara.bUse);
	//		m_pTableLabel->SetFieldValue("iChiBu",pInt->stAxisCtrlKp[i].stChiBuVal.iChiBu);
	//		m_pTableLabel->SetFieldValue("bPosMonitor",pInt->stAxisCtrlKp[i].stSoftPara.bPosMonitor);
	//		m_pTableLabel->SetFieldValue("fRasterLinear",(float)pInt->stAxisCtrlKp[i].stSoftPara.iRasterLinear);
	//		m_pTableLabel->SetFieldValue("iPosPrecision",pInt->stAxisCtrlKp[i].stSoftPara.iPosPrecision);
	//		m_pTableLabel->SetFieldValue("bLimitNoUse",pInt->stAxisCtrlKp[i].stSoftPara.bLimitNoUse);
	//		m_pTableLabel->SetFieldValue("iZeroMakeUp",pInt->stAxisCtrlKp[i].stSoftPara.iZeroMakeUp);
	//		m_pTableLabel->Update();
	//		m_pTableLabel->MoveNext();
	//	}
}

void EDM_Db::GetWorkPosSetIndex(int &iIndex)
{
#ifdef SQL
	int i = 0;
	QString strTmp;
	strTmp = QString("select workindex from total");
	q.exec(strTmp);
	while (q.next())
	{
		iIndex = q.value(0).toInt();
		i++;
	}
#else
	iIndex = 0;
#endif
}

void EDM_Db::SaveWorkPosSetIndex(int iIndex)
{
	//	m_pTableTotal->MoveFirst();
	//	m_pTableTotal->SetFieldValue("workindex",iIndex);
	//	m_pTableTotal->Update();
	//	m_pTableTotal->MoveFirst();
}

void EDM_Db::GetAllCoor(int a[][6])
{
#ifdef SQL	
	int i = 0;
	QString strTmp;
	strTmp = QString("select iRelSet_0,iRelSet_1,iRelSet_2,iRelSet_3,iRelSet_4,iRelSet_5 from label_var");
	q.exec(strTmp);
	while (q.next())
	{
		a[i][0] = q.value(0).toInt();
		a[i][1] = q.value(1).toInt();
		a[i][2] = q.value(2).toInt();
		a[i][3] = q.value(3).toInt();
		a[i][4] = q.value(4).toInt();
		a[i][5] = q.value(5).toInt();
		i++;
	}
#else
	a[0][0] = 0;
	a[0][1] = 0;
	a[0][2] = 0;
	a[0][3] = 0;
	a[0][4] = 0;
	a[0][5] = 0;
#endif
}

void EDM_Db::GetOpName(QString &str)
{
#ifdef SQL
	int i = 0;
	QString strTmp;
	strTmp = QString("select filename from total");
	q.exec(strTmp);
	while (q.next())
	{
		str = q.value(0).toString();
		i++;
	}
#endif
	str = "edmzhuanli";
}

void EDM_Db::SaveOpName(QString strName)
{
	//	m_pTableTotal->MoveFirst();
	//	m_pTableTotal->SetFieldValue("filename",strName);
	//	m_pTableTotal->Update();
	//	m_pTableTotal->MoveFirst();
}

void EDM_Db::SaveMacSystemPara(MAC_SYSTEM_SET *pSet)
{
	//   QString str,strTmp;
	//	int iVal[40] = {0};
	//	int iCnt=0;

	//	iCnt = sizeof(Mac_System_Set_None_Label)/sizeof(int);
	//	memcpy(iVal,&pSet->stSetNoneLabel,sizeof(Mac_System_Set_None_Label));
	//	str = "";
	//	for (int i=0;i<iCnt;i++)
	//	{
	//		strTmp.Format("%d ",iVal[i]);
	//		str +=strTmp;
	//	}
	//	str.Trim();
	//	m_pTableTotal->MoveFirst();
	//	m_pTableTotal->SetFieldValue("total",str);
	//	m_pTableTotal->Update();
	//	m_pTableTotal->MoveFirst();
}

void EDM_Db::GetMacSystemPara(MAC_KPINT *pInt, MAC_SYSTEM_SET *pSet)
{
#ifdef SQL
       QString strCmd,str;
        int iSet[40];
        int iLen;
        int i=0;
        int j=0;
        QChar ch;
        str = QString("select total,iAxisLabel from total");
        q.exec(str);
        if(q.next())
		{
            strCmd = q.value(0).toString();
			pSet->iAxistLabel = q.value(1).toInt();
		}
        
        strCmd.trimmed();
        iLen = strCmd.length();
        while (iLen>0)
        {
            for(i=0;i<iLen;i++)
            {
                ch = strCmd.at(i);
                if (ch==' ')
                {
                    str = strCmd.left(i);
                    str.trimmed();
                    iSet[j++]= str.toInt();
                    strCmd = strCmd.right(iLen-i);
                    strCmd.trimmed();
                    break;
                }
            }

            if (i >=iLen)
            {
                iSet[j++]= strCmd.toInt();
                strCmd="";
            }                                                                                                                                                                           
            iLen = strCmd.length();
        }
        memcpy(&pSet->stSetNoneLabel,iSet,sizeof(Mac_System_Set_None_Label));
        pInt->bDebug = pSet->stSetNoneLabel.bDebug;
        pInt->iOpLabel = pSet->stSetNoneLabel.iOpLabel;
        pInt->bOpDir = pSet->stSetNoneLabel.bOpDir;
#else
	pSet->iAxistLabel = 0;
	pInt->bDebug = pSet->stSetNoneLabel.bDebug = 0;
	pInt->iOpLabel = pSet->stSetNoneLabel.iOpLabel = 0;
	pInt->bOpDir = pSet->stSetNoneLabel.bOpDir = 0;
#endif
}

void EDM_Db::GetPrunePara(MAC_OTHER *pPrune)
{
#ifdef SQL 
	   QString strCmd,str;
        int iSet[10];
        int iLen;
        int i=0;
        int j=0;
        QChar ch;
        str = QString("select prune from total");
        q.exec(str);
        if(q.next())
		{
            strCmd = q.value(0).toString();
		}
		
        iLen = strCmd.length();
        while (iLen>0)
        {
            for(i=0;i<iLen;i++)
            {
                ch = strCmd.at(i);
                if (ch==' ')
                {
                    str = strCmd.left(i);
                    str.trimmed();
                    iSet[j++]= str.toInt();
                    strCmd = strCmd.right(iLen-i);
                    strCmd.trimmed();
                    break;
                }
            }

            if (i >=iLen)
            {
                iSet[j++]= strCmd.toInt();
                strCmd="";
            }
            iLen = strCmd.length();
        }
#endif
        memcpy(pPrune,0,sizeof(MAC_OTHER));
}
