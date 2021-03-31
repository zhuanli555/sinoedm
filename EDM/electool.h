#ifndef ELECTOOL_H
#define ELECTOOL_H

#include "common.h"
#define MAX(x,y) (x)>(y)?(x):(y)
#define MIN(x,y) (x)<(y)?(x):(y)

unsigned char  GetElecToffVal(int iElecToff);        //��Toff����
unsigned char  GetElecTonVal(int iElecTon);          //��ȡTon����
unsigned char  GetElecCurLowVal(int iElecCur);       //��ȡ��ѹ��������
unsigned char  GetElecCurHighVal(int iElecCur);      //��ȡ��ѹ��������

#endif // ELECTOOL_H
