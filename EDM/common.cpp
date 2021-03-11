#include "common.h"
/**
 * ÕýÏÞ¸ºÏÞ
 * iLabel£ºÖá±êÖ¾ºÅ 0:XÖá£»1:YÖá£»2:CÖá£»3:wÖá£»4:AÖá£»5:BÖá£»6:zÖá£»
 * bDir:1ÎªÕýÏÞ£¬0Îª¸ºÏÞ
*/
unsigned long CalcLimitBool(MAC_INTERFACE_IN *pIn,int iLabel,unsigned long bDir,unsigned long bDirectMotor)
{
	unsigned long bLimit=1;
	static unsigned char btLimit[MAC_LABEL_COUNT][2] = {{0x02,0x01}//x
														,{0x08,0x04}//y
														,{0x20,0x10}//c
														,{0x01,0x40}//w
														,{0x20,0x10}//A
														,{0x80,0x40}};//B
	switch(iLabel)
	{
	case 0://X
	case 1://y
	case 2://c
		bLimit=pIn->btI184 & btLimit[iLabel][bDir];
		break;	
	case 3://w
		{
			if (bDir)
				bLimit=pIn->btI184 & btLimit[iLabel][bDir];
			else
				bLimit=pIn->btI188 & btLimit[iLabel][bDir];
		}
		break;	
	case 4://A
	case 5://B
		bLimit=pIn->btI1C4 & btLimit[iLabel][bDir];
		break;
	default:
		break;
	}
	return bLimit;
}

//ÊäÈë£ºiLabel£ºÖá±êÖ¾ºÅ 0:XÖá£»1:YÖá£»2:ZÖá£»3:CÖá£»4:AÖá£»5:UÖá£»6:RÖá;2020
//iLabel£ºÖá±êÖ¾ºÅ 0:XÖá£»1:YÖá£»2:CÖá£»3:wÖá£»4:AÖá£»5:BÖá£»6:zÖá£»2021
unsigned long CalcAlarmBool(MAC_INTERFACE_IN *pIn,int iLabel)
{
	unsigned long bAlarm=1;
	static unsigned char btAlarm[MAC_LABEL_COUNT] = {0x01,0x02,0x40,0x80,0x01,0x02};
	switch(iLabel)
	{
	case 0://X Y
	case 1:
		bAlarm = pIn->btI148 & btAlarm[iLabel];
		break;
	case 2://c w
	case 3:
		bAlarm = pIn->btI144 & btAlarm[iLabel];		
		break;
	case 4://A B
	case 5:
		bAlarm = pIn->btI1C8 & btAlarm[iLabel];
		break;
	default:
		break;
	}
	return bAlarm;
}


unsigned long CalcDirectBool(MAC_INTERFACE_IN *pIn)
{
	unsigned long bDirect = 1;	

	bDirect = pIn->btI188 & 0x04;

	return bDirect;//¶ÌÂ·BOOLÖµ
}

unsigned long CalcCheckBool(MAC_INTERFACE_IN *pIn)
{
	unsigned long bCheck = 1;

	bCheck = !(pIn->btI144 & 0x08);

	return bCheck;
}
