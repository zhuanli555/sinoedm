//EDM_Db.cpp EDM_Db实现文件

#include "EDM_Db.h"
#include "cmdhandle.h"

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
    int i =0 ;
    for(;i<MAC_LABEL_COUNT;i++)
    {
        pComm->stMoveCtrlComm[i].iMachPos = 0;
        pComm->stMoveCtrlComm[i].bDirMove = 1;
        pComm->stMoveCtrlComm[i].iWorkPosSet = 0;
    }

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
	int i = 0;
    char chs[MAC_LABEL_COUNT]={'X','Y','C','W','A','B','Z'};
	for(;i<MAC_LABEL_COUNT;i++)
	{
        pInt->stAxisCtrlKp[i].stSoftPara.cSymbol = chs[i];
        pInt->stAxisCtrlKp[i].stSoftPara.bUse = 1;
        pInt->stAxisCtrlKp[i].stChiBuVal.iChiBu = 2;
        pInt->stAxisCtrlKp[i].stSoftPara.bPosMonitor = 0;
        pInt->stAxisCtrlKp[i].stSoftPara.iRasterLinear = 0;
        pInt->stAxisCtrlKp[i].stSoftPara.iPosPrecision = 0;
        pInt->stAxisCtrlKp[i].stSoftPara.bLimitNoUse = 0;
        pInt->stAxisCtrlKp[i].stSoftPara.iZeroMakeUp = 0;
        pInt->stAxisCtrlKp[i].stSoftPara.iLowerPos = 0;
        pInt->stAxisCtrlKp[i].stSoftPara.iTopPos = 400000;
        pInt->stAxisCtrlKp[i].stSoftPara.iLocateArea = 0;
        pInt->stAxisCtrlKp[i].stSoftPara.bDirectMotor = 0;

        pSet->iSetChiBu[i] = pInt->stAxisCtrlKp[i].stChiBuVal.iChiBu;
        pSet->bSetUse[i] = pInt->stAxisCtrlKp[i].stSoftPara.bUse;
        pSet->bSetLimitNoUse[i] = pInt->stAxisCtrlKp[i].stSoftPara.bLimitNoUse;
        pSet->iSetZeroMakeUp[i] = pInt->stAxisCtrlKp[i].stSoftPara.iZeroMakeUp;
        pSet->bPosMonitor[i] = pInt->stAxisCtrlKp[i].stSoftPara.bPosMonitor;
        pSet->iRasterLinear[i] = pInt->stAxisCtrlKp[i].stSoftPara.iRasterLinear;
        pSet->iPosPrecision[i] = pInt->stAxisCtrlKp[i].stSoftPara.iPosPrecision;
	}
	
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
	QString strTmp;
    strTmp = QString("select workindex from total limit 1");
	q.exec(strTmp);
	while (q.next())
	{
        iIndex = q.value(0).toInt();
    }
}

void EDM_Db::SaveWorkPosSetIndex(int iIndex)
{
    QString str;
    str = QString("update total set workindex=%1").arg(iIndex);
    q.exec(str);
}

void EDM_Db::GetAllCoor(int a[][6])
{
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
}

void EDM_Db::GetOpName(QString &str)
{
	QString strTmp;
    strTmp = QString("select filename from total where iIndex=0");
	q.exec(strTmp);
	while (q.next())
	{
        str = q.value(0).toString();
    }
}

void EDM_Db::SaveOpName(QString strName)
{
    QString str;
    str = QString("update total set filename='%1'").arg(strName);
    q.exec(str);
}

void EDM_Db::GetElecMan(MAP_ELEC_MAN* pElecMan)
{
    QString strTmp,sql;
    MAC_ELEC_PARA stMacElecPara;
    sql = QString("select * from elec");
    q.exec(sql);
    while (q.next())
    {
        memset(&stMacElecPara,0,sizeof(MAC_ELEC_PARA));
        stMacElecPara.iParaIndex = q.value(0).toInt();
        strTmp = q.value(1).toString();
        CmdHandle::GetElecPageParaFromQString(q.value(2).toString(),&(stMacElecPara.stElecPage[0]));
        CmdHandle::GetElecPageParaFromQString(q.value(3).toString(),&(stMacElecPara.stElecPage[1]));
        CmdHandle::GetElecPageParaFromQString(q.value(4).toString(),&(stMacElecPara.stElecPage[2]));
        CmdHandle::GetElecPageParaFromQString(q.value(5).toString(),&(stMacElecPara.stElecPage[3]));
        CmdHandle::GetElecPageParaFromQString(q.value(6).toString(),&(stMacElecPara.stElecPage[4]));
        CmdHandle::GetElecPageParaFromQString(q.value(7).toString(),&(stMacElecPara.stElecPage[5]));
        CmdHandle::GetElecOralParaFromQString(q.value(8).toString(),&(stMacElecPara.stElecOral));
        pElecMan->insert(pair<QString,MAC_ELEC_PARA>(strTmp,stMacElecPara));
    }
}


void EDM_Db::SaveElecMan(QString str,MAC_ELEC_PARA* pElec)
{
    QString sql;
    QString str0,str1,str2,str3,str4,str5,strOral;
    str0 = CmdHandle::GetElecPagePara2QString(&pElec->stElecPage[0]);
    str1 = CmdHandle::GetElecPagePara2QString(&pElec->stElecPage[1]);
    str2 = CmdHandle::GetElecPagePara2QString(&pElec->stElecPage[2]);
    str3 = CmdHandle::GetElecPagePara2QString(&pElec->stElecPage[3]);
    str4 = CmdHandle::GetElecPagePara2QString(&pElec->stElecPage[4]);
    str5 = CmdHandle::GetElecPagePara2QString(&pElec->stElecPage[5]);
    strOral = CmdHandle::GetElecOralPara2QString(&pElec->stElecOral);
    sql = QString("update elec set elec_page_0='%1',elec_page_1='%2',elec_page_2='%3',elec_page_3='%4',\
elec_page_4='%5',elec_page_5='%6',elec_oral='%7' where eName='%8'").arg(str0).arg(str1).arg(str2).arg(str3).arg(str4).arg(str5).arg(strOral).arg(str);
    q.exec(sql);
}


void EDM_Db::NewElecElem(QString str)
{
    QString sql;
    sql = QString("insert into elec(eName) values('%1')").arg(str);
    q.exec(sql);
}


void EDM_Db::DelElecMan(QString str)
{
    QString sql;
    sql = QString("delete from elec where eName='%1").arg(str);
    q.exec(sql);
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
