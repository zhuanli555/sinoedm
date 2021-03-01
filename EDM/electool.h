#ifndef ELECTOOL_H
#define ELECTOOL_H

#include "common.h"
#define MAX(x,y) (x)>(y)?(x):(y)
#define MIN(x,y) (x)<(y)?(x):(y)

int GetElecToff(int iElecToff,int* pIndex);        //获取Toff
int GetElecTon(int iElecTon,int* pIndex);          //获取Ton
int GetElecCurLow(int iElecCur,int* pIndex);       //获取低压电流
int GetElecCurHigh(int iElecCur,int* pIndex);      //获取高压电流
int GetElecCap(int iCap,int* pIndex);              //获取电容

int GetElecToffByIndex(int iIndex);         //获取Toff
int GetElecTonByIndex(int iIndex);          //获取Ton
int GetElecCurLowByIndex(int iIndex);       //获取低压电流
int GetElecCurHighByIndex(int iIndex);      //获取高压电流
int GetElecCapByIndex(int iIndex);          //获取电容

unsigned char  GetElecToffVal(int iElecToff);        //获Toff编码
unsigned char  GetElecTonVal(int iElecTon);          //获取Ton编码
unsigned char  GetElecCurLowVal(int iElecCur);       //获取低压电流编码
unsigned char  GetElecCurHighVal(int iElecCur);      //获取高压电流编码
unsigned char  GetElecCapVal(int iCap);              //获取电容编码


#endif // ELECTOOL_H
