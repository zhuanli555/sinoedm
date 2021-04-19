#pragma once

#include "common.h"
#include <QString>
#include <QtSql>
#include <QMessageBox>
#include <QMap>
#include <vector>

using namespace std;
typedef QMap<QString ,MAC_ELEC_PARA> MAP_ELEC_MAN;

class EDM_Db
{
public:
    EDM_Db();
	void GetAxisLuoBuData(char cSymbol,int iVal[],int nCount);
    void GetEdmCommPara(MAC_COMMON* pComm,int iWorkIndex);
    void SaveEdmCommPara(MAC_COMMON* pComm,int iWorkIndex,int a[][6]);
    void SaveEdmWorkSet(int iWorkIndex,int a[][6]);

    void GetEdmKpIntPara(MAC_KPINT* pInt,MAC_SYSTEM_SET* pSet);
    void SaveEdmKpIntPara(MAC_KPINT* pInt,MAC_SYSTEM_SET* pSet);

    void GetOpName(QString& str);
    void SaveOpName(QString strName);

    void GetElecMan(MAP_ELEC_MAN* pElecMan);
    void SaveElecMan(QString str,MAC_ELEC_PARA* pElec);
    void DelElecMan(QString str);
    void NewElecElem(QString str,MAC_ELEC_PARA* pElec);

    void GetWorkPosSetIndex(int& iIndex);
    void SaveWorkPosSetIndex(int iIndex);
    void GetAllCoor(int a[][6]);

    void SaveMacSystemPara(MAC_SYSTEM_SET* pSet);
    void GetMacSystemPara(MAC_KPINT* pInt,MAC_SYSTEM_SET* pSet);

    void GetPrunePara(MAC_OTHER* pPrune);
private:	
    QSqlError initDb();
    QSqlDatabase db;
    QSqlQuery q;
    QString GetElecPagePara2QString(Elec_Page* pElecPage);
    QString GetElecOralPara2QString(Elec_Oral* pElecOral);
    QString GetElecPageParaFromQString(QString strCmd,Elec_Page* pElecPage);
    QString GetElecOralParaFromQString(QString strCmd,Elec_Oral* pElecOral);
};
