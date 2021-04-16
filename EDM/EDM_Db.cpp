//EDM_Db.cpp EDM_Db实现文件

#include "EDM_Db.h"
#include "cmdhandle.h"

EDM_Db::EDM_Db()
{
    initDb();
}

QSqlError EDM_Db::initDb()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("sino.db");
    q = QSqlQuery(db);
    if (!db.open()) {
        QMessageBox::critical(nullptr, QObject::tr("Cannot open database"),
            QObject::tr("Unable to establish a database connection.\n"
                        "This example needs SQLite support. Please read "
                        "the Qt SQL driver documentation for information how "
                        "to build it.\n\n"
                        "Click Cancel to exit."), QMessageBox::Cancel);
        return db.lastError();
    }

   QStringList tables = db.tables();
   if (tables.contains("elec", Qt::CaseInsensitive))
   {
       return QSqlError();
   }

    q.exec(QLatin1String("create table x_label(iIndex integer primary key, iPos integer,iVal integer)"));
    q.exec(QLatin1String("create table y_label(iIndex integer primary key, iPos integer,iVal integer)"));
    q.exec(QLatin1String("create table z_label(iIndex integer primary key, iPos integer,iVal integer)"));
    q.exec(QLatin1String("create table a_label(iIndex integer primary key, iPos integer,iVal integer)"));
    q.exec(QLatin1String("create table b_label(iIndex integer primary key, iPos integer,iVal integer)"));
    q.exec(QLatin1String("create table c_label(iIndex integer primary key, iPos integer,iVal integer)"));
    q.exec(QLatin1String("create table label(iIndex integer primary key, cSymbol varchar(4),bUse integer,bRotateLabel integer,iChibu integer,bPosMonitor integer,fRasterLinear integer,iPosPrecision integer,bLimitNoUse Integer,iZeroMakeUp integer,iLowerPos integer,iTopPos integer,iLocateArea integer,bDirectMotor integer)"));
    q.exec(QLatin1String("create table label_var(iIndex integer primary key, iMachPos integer,bDir integer,iRelSet_0 integer,iRelSet_1 integer,iRelSet_2 integer,iRelSet_3 integer,iRelSet_4 integer,iRelSet_5 integer)"));
    q.exec(QLatin1String("create table total(iIndex integer primary key, filename varchar(32),workindex integer,prune varchar(128),total varchar(255),iAxisLabel integer)"));
    q.exec(QLatin1String("create table elec(iIndex integer primary key,eName varchar(24),elec_page_0 varchar(64),elec_page_1 varchar(64),elec_page_2 varchar(64),elec_page_3 varchar(64),elec_page_4 varchar(64),elec_page_5 varchar(64),elec_oral varchar(128))"));

    return QSqlError();
}

void EDM_Db::GetAxisLuoBuData(char cSymbol, int iVal[], int nCount)
{
#ifdef SQL	
	QString str;

    str = QString(QLatin1String("SELECT * FROM %c_label order by iIndex")).arg(cSymbol);
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
    int i = 0;
    QString strTmp;

    strTmp = QString("select iMachPos,bDir,iRelSet_%1 from label_var order by iIndex").arg(iWorkIndex);
    q.exec(strTmp);
    while (q.next())
	{
        pComm->stMoveCtrlComm[i].iMachPos = q.value(0).toInt();
        pComm->stMoveCtrlComm[i].bDirMove = q.value(1).toInt();
        pComm->stMoveCtrlComm[i].iWorkPosSet = q.value(2).toInt();
		i++;
    }
}

void EDM_Db::SaveEdmCommPara(MAC_COMMON *pComm, int iWorkIndex, int a[][6])
{
    QString str;

    for(int i =0;i<MAC_LABEL_COUNT;i++)
    {
        str = QString("update label_var set iMachPos=%1,bDir=%2,iRelSet_%3=%4 where iIndex=%5").arg(pComm->stMoveCtrlComm[i].iMachPos)\
                .arg(pComm->stMoveCtrlComm[i].bDirMove)\
                .arg(iWorkIndex).arg(a[i][iWorkIndex]).arg(i);
        q.exec(str);
    }
}

void EDM_Db::SaveEdmWorkSet(int iWorkIndex, int a[][6])
{
    QString str;

    for(int i =0;i<MAC_LABEL_COUNT;i++)
    {
        str = QString("update label_var set iRelSet_%1=%2 where iIndex=%3").arg(iWorkIndex).arg(a[i][iWorkIndex]).arg(i);
        q.exec(str);
    }
}

void EDM_Db::GetEdmKpIntPara(MAC_KPINT *pInt, MAC_SYSTEM_SET *pSet)
{
	int i = 0;
	QString strTmp;

	strTmp = QString("select cSymbol,bUse,bRotateLabel,iChiBu,bPosMonitor,fRasterLinear,iPosPrecision,bLimitNoUse,iZeroMakeUp,\
iLowerPos,iTopPos,iLocateArea,bDirectMotor from label order by iIndex");
    q.exec(strTmp);
    while (q.next())
	{
        pInt->stAxisCtrlKp[i].stSoftPara.cSymbol = q.value(0).toChar().unicode();
        pInt->stAxisCtrlKp[i].stSoftPara.bUse = q.value(1).toInt();
        pInt->stAxisCtrlKp[i].stSoftPara.bRotateLabel = q.value(2).toInt();
        pInt->stAxisCtrlKp[i].stChiBuVal.iChiBu = q.value(3).toInt();
        pInt->stAxisCtrlKp[i].stSoftPara.bPosMonitor = q.value(4).toInt();
        pInt->stAxisCtrlKp[i].stSoftPara.iRasterLinear = q.value(5).toInt();
        pInt->stAxisCtrlKp[i].stSoftPara.iPosPrecision = q.value(6).toInt();
        pInt->stAxisCtrlKp[i].stSoftPara.bLimitNoUse = q.value(7).toInt();
        pInt->stAxisCtrlKp[i].stSoftPara.iZeroMakeUp = q.value(8).toInt();
        pInt->stAxisCtrlKp[i].stSoftPara.iLowerPos = q.value(9).toInt();
        pInt->stAxisCtrlKp[i].stSoftPara.iTopPos = q.value(10).toInt();
        pInt->stAxisCtrlKp[i].stSoftPara.iLocateArea = q.value(11).toInt();
        pInt->stAxisCtrlKp[i].stSoftPara.bDirectMotor = q.value(12).toInt();

		pSet->iSetChiBu[i] = pInt->stAxisCtrlKp[i].stChiBuVal.iChiBu;
		pSet->bSetUse[i] = pInt->stAxisCtrlKp[i].stSoftPara.bUse;
		pSet->bSetLimitNoUse[i] = pInt->stAxisCtrlKp[i].stSoftPara.bLimitNoUse;
		pSet->iSetZeroMakeUp[i] = pInt->stAxisCtrlKp[i].stSoftPara.iZeroMakeUp;
		pSet->bPosMonitor[i] = pInt->stAxisCtrlKp[i].stSoftPara.bPosMonitor;
		pSet->iRasterLinear[i] = pInt->stAxisCtrlKp[i].stSoftPara.iRasterLinear;
		pSet->iPosPrecision[i] = pInt->stAxisCtrlKp[i].stSoftPara.iPosPrecision;
		i++;
    }
}

void EDM_Db::SaveEdmKpIntPara(MAC_KPINT *pInt, MAC_SYSTEM_SET *pSet)
{
    QString str;

    for(int i =0;i<MAC_LABEL_COUNT;i++)
    {
        pInt->stAxisCtrlKp[i].stSoftPara.bUse = pSet->bSetUse[i];
        pInt->stAxisCtrlKp[i].stChiBuVal.iChiBu = pSet->iSetChiBu[i];
        pInt->stAxisCtrlKp[i].stSoftPara.bPosMonitor = pSet->bPosMonitor[i];
        pInt->stAxisCtrlKp[i].stSoftPara.iRasterLinear = pSet->iRasterLinear[i];
        pInt->stAxisCtrlKp[i].stSoftPara.iPosPrecision = pSet->iPosPrecision[i];
        pInt->stAxisCtrlKp[i].stSoftPara.bLimitNoUse = pSet->bSetLimitNoUse[i];
        pInt->stAxisCtrlKp[i].stSoftPara.iZeroMakeUp = pSet->iSetZeroMakeUp[i];
        str = QString("update label set bUse=%1,iChiBu=%2,bPosMonitor=%3,fRasterLinear=%4,iPosPrecision=%5,\
                      bLimitNoUse=%6,iZeroMakeUp=%7 where iIndex=%8").arg(pInt->stAxisCtrlKp[i].stSoftPara.bUse)\
                .arg(pInt->stAxisCtrlKp[i].stChiBuVal.iChiBu)\
                .arg(pInt->stAxisCtrlKp[i].stSoftPara.bPosMonitor)\
                .arg(pInt->stAxisCtrlKp[i].stSoftPara.iRasterLinear)\
                .arg(pInt->stAxisCtrlKp[i].stSoftPara.iPosPrecision)\
                .arg(pInt->stAxisCtrlKp[i].stSoftPara.bLimitNoUse)\
                .arg(pInt->stAxisCtrlKp[i].stSoftPara.iZeroMakeUp).arg(i);
        q.exec(str);
    }
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
	strTmp = QString("select iRelSet_0,iRelSet_1,iRelSet_2,iRelSet_3,iRelSet_4,iRelSet_5 from label_var order by iIndex");
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
    sql = QString("select iIndex,eName,elec_page_0,elec_page_1,elec_page_2,\
elec_page_3,elec_page_4,elec_page_5,elec_oral from elec");
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
                                    elec_page_4='%5',elec_page_5='%6',elec_oral='%7' where eName='%8'").arg(str0).arg(str1)\
                .arg(str2).arg(str3).arg(str4).arg(str5).arg(strOral).arg(str);

                bool a = q.exec(sql);
        qDebug()<<sql<<a;


//    sql = QString("insert into elec(eName,elec_page_0,elec_page_1,elec_page_2,elec_page_3,\
//                                elec_page_4,elec_page_5,elec_oral) values('%1','%2','%3','%4','%5','%6','%7','%8')").arg(str)\
//                  .arg(str0).arg(str1).arg(str2).arg(str3).arg(str4).arg(str5).arg(strOral);
//            qDebug()<<sql;
//    q.exec(sql);

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
    QString str,strTmp;
    int iVal[40] = {0};
    int iCnt=0;


    iCnt = sizeof(Mac_System_Set_None_Label)/sizeof(int);
    memcpy(iVal,&pSet->stSetNoneLabel,sizeof(Mac_System_Set_None_Label));
    str = "";
    for (int i=0;i<iCnt;i++)
    {
        strTmp = QString("%1 ").arg(iVal[i]);
        str += strTmp;
    }
    str = str.trimmed();
    strTmp = QString("update total set total='%1' where iIndex=0").arg(str);
    q.exec(strTmp);
}

void EDM_Db::GetMacSystemPara(MAC_KPINT *pInt, MAC_SYSTEM_SET *pSet)
{
    QString strCmd,str;
    int iSet[40];
    int i=0;

    str = QString("select total,iAxisLabel from total limit 1");
    q.exec(str);
    if(q.next())
    {
        strCmd = q.value(0).toString();
        pSet->iAxistLabel = q.value(1).toInt();
    }
    QStringList list = strCmd.split(' ');
    foreach (const QString& str, list) {
        iSet[i++] = str.toInt();
    }
    memcpy(&pSet->stSetNoneLabel,iSet,sizeof(Mac_System_Set_None_Label));
    pInt->bDebug = pSet->stSetNoneLabel.bDebug;
    pInt->iOpLabel = pSet->stSetNoneLabel.iOpLabel;
    pInt->bOpDir = pSet->stSetNoneLabel.bOpDir;
}

void EDM_Db::GetPrunePara(MAC_OTHER *pPrune)
{
    QString strCmd,str;
    int iSet[10] = {0};
    int i=0;

    str = QString("select prune from total limit 1");
    q.exec(str);
    if(q.next())
    {
        strCmd = q.value(0).toString();
    }
    QStringList list = strCmd.split(' ');
    foreach (const QString& str, list) {
        iSet[i++] = str.toInt();
    }
    memcpy(pPrune,iSet,sizeof(MAC_OTHER));
}
