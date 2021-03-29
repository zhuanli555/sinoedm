#include "electool.h"

float g_iElecToff[14] ={0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5};
unsigned char g_btElecToffVal[14] ={0x0E,0x0D,0x0C,0x0B,0x0A,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01};

float g_iElecTon[14] ={0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5};
unsigned char g_btElecTonVal[14] ={0x0E,0x0D,0x0C,0x0B,0x0A,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01};

unsigned char g_btElecLowVal[12] = {0xFF,0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F,0x7F,0x7F,0x7F};

unsigned char g_btElecHighVal[9] = {0xFF,0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};//加工电流

int g_iElecCap[8] = {0,5,10,15,20,25,30,35};//电容
unsigned char g_btElecCapVal[8] = {0x7,0x6,0x5,0x4,0x3,0x2,0x1,0x0};

//-------------------------------------------------------------------------------------------------

//获取Toff编码
unsigned char   GetElecToffVal(float iElecToff)
{
    int i=0;

    for (;i<14;i++)
    {
        if (iElecToff- g_iElecToff[i] < 0.001 && iElecToff- g_iElecToff[i] > -0.001)
        {
            return g_btElecToffVal[i];
        }
    }
}

//获取Ton编码
unsigned char   GetElecTonVal(float iElecTon)
{
    int i=0;

    for (;i<14;i++)
    {
        if (iElecTon- g_iElecToff[i] < 0.001 && iElecTon- g_iElecToff[i] > -0.001)
        {
            return g_btElecToffVal[i];
        }
    }
}

//获取加工电流编码
unsigned char   GetElecCurLowVal(int iElecLow)
{
    if(iElecLow>11)iElecLow = 11;
    if(iElecLow<0)iElecLow = 0;
    return g_btElecLowVal[iElecLow];
}

//获取高压电流编码
unsigned char   GetElecCurHighVal(int iElecHigh)
{
    if(iElecHigh>8)iElecHigh = 8;
    if(iElecHigh<0)iElecHigh = 0;

    return g_btElecHighVal[iElecHigh];
}
