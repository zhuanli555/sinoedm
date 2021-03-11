#include "common.h"
/**
 * ���޸���
 * iLabel�����־�� 0:X�᣻1:Y�᣻2:C�᣻3:w�᣻4:A�᣻5:B�᣻6:z�᣻
 * bDir:1Ϊ���ޣ�0Ϊ����
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

//���룺iLabel�����־�� 0:X�᣻1:Y�᣻2:Z�᣻3:C�᣻4:A�᣻5:U�᣻6:R��;2020
//iLabel�����־�� 0:X�᣻1:Y�᣻2:C�᣻3:w�᣻4:A�᣻5:B�᣻6:z�᣻2021
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

	return bDirect;//��·BOOLֵ
}

unsigned long CalcCheckBool(MAC_INTERFACE_IN *pIn)
{
	unsigned long bCheck = 1;

	bCheck = !(pIn->btI144 & 0x08);

	return bCheck;
}
