#pragma once

#include "EDM.h"
#include "cmdhandle.h"
#include "EDM_OP_File.h"

typedef enum
{
	OP_NO_ERR = 0,             //����
	OP_HARD_ERR ,              //Ӳ������
	OP_EXCEED_TIME ,           //��ʱ����
	OP_LEN_POLE,               //�缫���Ȳ���
}OP_ERROR;

typedef struct  
{
	OP_ERROR errOp;
	OP_FILE_ERROR errFile;
}ERROR_ALL;

typedef struct opPassChartPara
{
	BOOL bClear;
	BOOL bSet;
	BOOL bRealTimeIn;
	int iPosRealTime;
	int iSpeedRealTime;
	int iElecRealTime;
	int iSpeedFactor;	
	int iPosJudge;
	int iElecJudge;
	int iSpeedJudge;
	float fElecFactor;
}Pass_Chart_Para;

typedef struct opStatusNewCycle
{
	int  iOpPage;              //�ӹ�ҳ
	int  iCycleIndex;          //ѭ�����
	int  iTimeSec;             //ʱ��
	int  iTimeCnt;             //���μӹ�ʱ�������
	BOOL bPauseCmd;            //��ָͣ��
	BOOL bCycleStart; 
	Pass_Chart_Para stPassChart;
}STATUS_NEW_CYCYLE;

typedef struct opPassParaSet
{
	float fElec;
	float fSpeed;
	int iSpeedFilterCnt;
	int iElecFilterCnt;
}PASS_PARA_SET;

typedef struct  opStatus
{
	MAC_OPERATE_TYPE enOpType; //�ӹ�����
	STATUS_NEW_CYCYLE stCycle; //״̬��Ϣ
	ERROR_ALL  enErrAll;       //������ʾ
	BOOL bStart;               //��ͣ
	BOOL bOpOver;              //���μӹ�����
	BOOL bCheck_C_Over;        //Բ�Ȳ������
	int  iCmdIndex;            //�����
	BOOL bNewOp;
	void* pNewHoleOp;
}OP_STATUS;

class EDM_OP
{
public:
	EDM_OP();
	virtual  ~EDM_OP();	
public:
	static BOOL m_bSetPower;
	static BOOL m_bStartCount;
	static BOOL m_bInOp;	
	EDM_OP* m_pEdmOpPre;
	OP_STATUS m_stOpStatus;	
	
	EDM_OP_File* m_pOpFile;
protected:
	EDM* m_pEdm;
	MAC_OPERATE_TYPE m_enOpType;
	QString m_sPath;
	int m_iWorkPos_All[6][MAC_LABEL_COUNT];
	int  m_iSafeLabelMacPos;
	int  m_iWholeFreq;
	BOOL m_bCalc;

	PASS_PARA_SET m_stPassSet;	
public:
	virtual void EdmOpSetStart(BOOL bStart)=0;
	virtual void EdmOpCarry()=0;
	virtual void EdmOpOver()=0;
	virtual void EdmOpStageRestart()=0;
	virtual void EdmOpSetTest(BOOL bTest)=0;
	
	void SetEdmOpFile(QString sPath,QString sFile);
	void CalcDigitCmd();
	void SetEdmOpElec(QString str,MAC_ELEC_PARA elec);
	void SetPassPara(float,float,int,int);
protected:
	BOOL EdmOpErr();
};


