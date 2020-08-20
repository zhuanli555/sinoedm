#pragma once

#include "common.h"
#include <QString>
#include <QtSql>
#include <map>
#include <vector>

using namespace std;
typedef map<QString ,MAC_ELEC_PARA> MAP_ELEC_MAN;

class EDM_Db
{
public:
	void GetAxisLuoBuData(char cSymbol,int iVal[],int nCount);

    void GetEdmCommPara(MAC_COMMON* pComm,int iWorkIndex);
    void SaveEdmCommPara(MAC_COMMON* pComm,int iWorkIndex,int a[][6]);
    void SaveEdmWorkSet(int iWorkIndex,int a[][6]);

    void GetEdmKpIntPara(MAC_KPINT* pInt,MAC_SYSTEM_SET* pSet);
    void SaveEdmKpIntPara(MAC_KPINT* pInt,MAC_SYSTEM_SET* pSet);

    void GetOpName(QString& str);
    void SaveOpName(QString strName);

    void GetWorkPosSetIndex(int& iIndex);
    void SaveWorkPosSetIndex(int iIndex);
    void GetAllCoor(int a[][6]);

    void SaveMacSystemPara(MAC_SYSTEM_SET* pSet);
    void GetMacSystemPara(MAC_KPINT* pInt,MAC_SYSTEM_SET* pSet);

    void GetPrunePara(MAC_OTHER* pPrune);
private:	
    QSqlQuery q;
};
