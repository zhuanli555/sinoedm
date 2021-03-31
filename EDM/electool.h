#ifndef ELECTOOL_H
#define ELECTOOL_H

#include "common.h"
#define MAX(x,y) (x)>(y)?(x):(y)
#define MIN(x,y) (x)<(y)?(x):(y)

unsigned char  GetElecToffVal(int iElecToff);        //获Toff编码
unsigned char  GetElecTonVal(int iElecTon);          //获取Ton编码
unsigned char  GetElecCurLowVal(int iElecCur);       //获取低压电流编码
unsigned char  GetElecCurHighVal(int iElecCur);      //获取高压电流编码

#endif // ELECTOOL_H
