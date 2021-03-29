#ifndef ELECTOOL_H
#define ELECTOOL_H

#include "common.h"
#define MAX(x,y) (x)>(y)?(x):(y)
#define MIN(x,y) (x)<(y)?(x):(y)

unsigned char  GetElecToffVal(float iElecToff);        //��Toff����
unsigned char  GetElecTonVal(float iElecTon);          //��ȡTon����
unsigned char  GetElecCurLowVal(int iElecCur);       //��ȡ��ѹ��������
unsigned char  GetElecCurHighVal(int iElecCur);      //��ȡ��ѹ��������

#endif // ELECTOOL_H
