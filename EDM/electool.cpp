#include "electool.h"

int g_iElecToff[16] ={12,14,16,18,20,24,30,36,42,48,54,60,70,80,90,100};
unsigned char g_btElecToffVal[16] ={0x0F,0x0E,0x0D,0x0C,0x0B,0x0A,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00};

int g_iElecTon[32] = {2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64};
unsigned char g_btElecTonVal[32] = {0x1F,0x1E,0x1D,0x1C,0x1B,0x1A,0x19,0x18,0x17,0x16,0x15,0x14,0x13,0x12,0x11,0x10,
0x0F,0x0E,0x0D,0x0C,0x0B,0x0A,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00};

//int g_iElecLow[24] = {0,1,2,3,4,5,6,7,8,10,12,16,20,25,30,35,40,45,50,60,70,80,90,100};
//unsigned char g_btElecLowVal[24] = {0x1F,0x1E,0x1D,0x1C,0x1B,0x1A,0x19,0x18,0x17,0x16,0x15,0x14,0x13,0x12,0x11,0x10,
//0x0F,0x0E,0x0D,0x0C,0x0B,0x0A,0x09,0x08};

int g_iElecLow[32] ={0,1,2,3,4,5,6,7,8,10,12,16,20,25,30,35,37,40,42,45,47,50,55,60,65,70,75,80,85,90,95,100};
unsigned char g_btElecLowVal[32] = {0x1F,0x1E,0x1D,0x1C,0x1B,0x1A,0x19,0x18,0x17,0x16,0x15,0x14,0x13,0x12,0x11,0x10,0x10,
                           0x0F,0x0F,0x0E,0x0E,0x0D,0x0D,0x0C,0x0C,0x0B,0x0B,0x0A,0x0A,0x09,0x09,0x08};

int g_iElecHigh[4] = {0,1,2,3};
unsigned char g_btElecHighVal[4] = {0x3,0x2,0x1,0x00};

int g_iElecCap[8] = {0,5,10,15,20,25,30,35}; //float g_fElecCap[8] = {0,0.05,0.1,0.15,0.2,0.25,0.3,0.35};
unsigned char g_btElecCapVal[8] = {0x7,0x6,0x5,0x4,0x3,0x2,0x1,0x0};

//-------------------------------------------------------------------------------------------------

//获取Toff编码
unsigned char   GetElecToffVal(int iElecToff)
{
    int i=0;

    for (;i<16;i++)
    {
        if (iElecToff<= g_iElecToff[i])
        {
            return g_btElecToffVal[i];
        }
    }

    return g_btElecToffVal[15];
}

//获取Ton编码
unsigned char   GetElecTonVal(int iElecTon)
{
    int i=31;

    for (;i>= 0;i--)
    {
        if (iElecTon >= g_iElecTon[i])
        {
            return g_btElecTonVal[i];
        }
    }

    return g_btElecTonVal[0];
}

//获取加工电流编码
unsigned char   GetElecCurLowVal(int iElecLow)
{
    int i=31;

    for (;i>= 0;i--)
    {
        if (iElecLow >= g_iElecLow[i])
        {
            return g_btElecLowVal[i];
        }
    }

    return g_btElecLowVal[0];
}

//获取高压电流编码
unsigned char   GetElecCurHighVal(int iElecHigh)
{
    int i=0;

    for (;i<4;i++)
    {
        if (iElecHigh<= g_iElecHigh[i])
        {
            return g_btElecHighVal[i];
        }
    }

    return g_btElecHighVal[3];
}

//获取电容编码
unsigned char   GetElecCapVal(int iCap)
{
    int i =0;

    for (;i<8;i++)
    {
        if (iCap<= g_iElecCap[i])
        {
            return g_btElecCapVal[i];
        }
    }

    return g_btElecCapVal[7];
}

//获取Toff数值
int  GetElecToff(int iElecToff,int* pIndex)
{
    int i=0;
    *pIndex = 15;

    for (;i<16;i++)
    {
        if (iElecToff<= g_iElecToff[i])
        {
            *pIndex = i;
            return g_iElecToff[i];
        }
    }

    return g_iElecToff[15];
}

//获取Ton数值
int  GetElecTon(int iElecTon,int* pIndex)
{
    int i=31;
    *pIndex =0;

    for (;i>= 0;i--)
    {
        if (iElecTon >= g_iElecTon[i])
        {
            *pIndex =i;
            return g_iElecTon[i];
        }
    }

    return g_iElecTon[0];
}

//获取电流数值
int  GetElecCurLow(int iElecLow,int* pIndex)
{
    int i=31;
    *pIndex = 0;

    for (;i>= 0;i--)
    {
        if (iElecLow >= g_iElecLow[i])
        {
            *pIndex = i;
            return g_iElecLow[i];
        }
    }

    return g_iElecLow[0];
}

//获取高压电流数值
int  GetElecCurHigh(int iElecHigh,int* pIndex)
{
    int i=0;
    *pIndex =3;

    for (;i<4;i++)
    {
        if (iElecHigh<= g_iElecHigh[i])
        {
            *pIndex =i;
            return g_iElecHigh[i];
        }
    }

    return g_iElecHigh[3];
}

//获取电容数值
int  GetElecCap(int iCap,int* pIndex)
{
    int i =0;
    *pIndex = 7;

    for (;i<8;i++)
    {
        if (iCap<= g_iElecCap[i])
        {
            *pIndex = i;
            return g_iElecCap[i];
        }
    }

    return g_iElecCap[7];
}

int GetElecToffByIndex(int iIndex)        //获取Toff
{
    iIndex = MAX(iIndex,0);
    iIndex = MIN(iIndex,15);

    if (iIndex>=0 && iIndex<=15)
    {
        return g_iElecToff[iIndex];
    }
    return g_iElecToff[0];
}

int GetElecTonByIndex(int iIndex)          //获取Ton
{
    iIndex = MAX(iIndex,0);
    iIndex = MIN(iIndex,31);

    if (iIndex>=0 && iIndex<=31)
    {
        return g_iElecTon[iIndex];
    }
    return g_iElecTon[0];
}
int GetElecCurLowByIndex(int iIndex)       //获取低压电流
{
    iIndex = MAX(iIndex,0);
    iIndex = MIN(iIndex,31);

    if (iIndex>=0 && iIndex<=31)
    {
        return g_iElecLow[iIndex];
    }
    return g_iElecLow[0];
}

int GetElecCurHighByIndex(int iIndex)      //获取高压电流
{
    iIndex = MAX(iIndex,0);
    iIndex = MIN(iIndex,3);

    if (iIndex>=0 && iIndex<=3)
    {
        return g_iElecHigh[iIndex];
    }
    return g_iElecHigh[0];
}

int GetElecCapByIndex(int iIndex)        //获取电容
{
    iIndex = MAX(iIndex,0);
    iIndex = MIN(iIndex,7);

    if (iIndex>=0 && iIndex<=7)
    {
        return g_iElecCap[iIndex];
    }
    return g_iElecCap[0];
}
