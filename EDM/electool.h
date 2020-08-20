#ifndef ELECTOOL_H
#define ELECTOOL_H

#include "common.h"
#define MAX(x,y) (x)>(y)?(x):(y)
#define MIN(x,y) (x)<(y)?(x):(y)

int GetElecToff(int iElecToff,int* pIndex);        //��ȡToff
int GetElecTon(int iElecTon,int* pIndex);          //��ȡTon
int GetElecCurLow(int iElecCur,int* pIndex);       //��ȡ��ѹ����
int GetElecCurHigh(int iElecCur,int* pIndex);      //��ȡ��ѹ����
int GetElecCap(int iCap,int* pIndex);              //��ȡ����

int GetElecToffByIndex(int iIndex);         //��ȡToff
int GetElecTonByIndex(int iIndex);          //��ȡTon
int GetElecCurLowByIndex(int iIndex);       //��ȡ��ѹ����
int GetElecCurHighByIndex(int iIndex);      //��ȡ��ѹ����
int GetElecCapByIndex(int iIndex);          //��ȡ����

BYTE  GetElecToffVal(int iElecToff);        //��Toff����
BYTE  GetElecTonVal(int iElecTon);          //��ȡTon����
BYTE  GetElecCurLowVal(int iElecCur);       //��ȡ��ѹ��������
BYTE  GetElecCurHighVal(int iElecCur);      //��ȡ��ѹ��������
BYTE  GetElecCapVal(int iCap);              //��ȡ���ݱ���


#endif // ELECTOOL_H
