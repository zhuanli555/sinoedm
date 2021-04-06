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

typedef struct opStatusNewCycle
{
	int  iOpPage;              //�ӹ�ҳ
	int  iCycleIndex;          //ѭ�����
	int  iTimeSec;             //ʱ��
	int  iTimeCnt;             //���μӹ�ʱ�������
	unsigned char bPauseCmd;            //��ָͣ��
    unsigned char bCycleStart;
}STATUS_NEW_CYCYLE;

typedef struct  opStatus
{
	MAC_OPERATE_TYPE enOpType; //�ӹ�����
	STATUS_NEW_CYCYLE stCycle; //״̬��Ϣ
    OP_ERROR errOp;
    OP_FILE_ERROR errFile;//������ʾ
	unsigned char bStart;               //��ͣ
	unsigned char bOpOver;              //���μӹ�����
	unsigned char bCheck_C_Over;        //Բ�Ȳ������
	int  iCmdIndex;            //�����
	unsigned char bNewOp;
	void* pNewHoleOp;
}OP_STATUS;

class EDM_OP
{
public:
	EDM_OP();
	virtual  ~EDM_OP();	
public:
	static unsigned char m_bSetPower;
	static unsigned char m_bStartCount;
	static unsigned char m_bInOp;	
	EDM_OP* m_pEdmOpPre;
	OP_STATUS m_stOpStatus;	

	EDM_OP_File* m_pOpFile;
protected:
    EDM* m_pEdm;
	MAC_OPERATE_TYPE m_enOpType;
	QString m_sPath;
	int m_iWorkPos_All[MAC_LABEL_COUNT][6];
	int  m_iSafeLabelMacPos;
	int  m_iWholeFreq;
	unsigned char m_bCalc;

public:
	virtual void EdmOpSetStart(unsigned char bStart)=0;
	virtual void EdmOpCarry()=0;
	virtual void EdmOpOver()=0;
    virtual void EdmOpStageRestart()=0;
	
	void SetEdmOpFile(QString sPath,QString sFile);
	void CalcDigitCmd();
    void SetEdmOpElec(QString str,MAC_ELEC_PARA elec);
protected:
	unsigned char EdmOpErr();
};


