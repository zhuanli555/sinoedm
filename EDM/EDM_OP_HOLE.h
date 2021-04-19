#pragma once

#include "EDM_OP.h"
#include <list>
using std::list;

//����ģʽ
typedef enum
{
	CHECK_DIRECT = 0,   //��·ģʽ
	CHECK_ELASTIC = 1,  //����ģʽ
	CHECK_ELEC  = 2,    //���ģʽ
}CHECK_MODE;

typedef struct holeZeroCtrl
{
	unsigned char bCycleStart;          //�׶ο�ʼ
	unsigned char bRepeatSec;           //ÿ���ظ����̵ĵڶ��׶�
	unsigned char bWait;                //�ڵȴ���е�źŵ��ź�ͬ��
	unsigned char bStageLast;           //ÿһ�׶ε����
	unsigned char bMillLast;
	unsigned char bEmptyMove;           //��������
	unsigned char bMillLenEnough;       //ϳ����ȵ���
	unsigned char bMillPause;
	unsigned char bSafeLabelExist;
	int  iCheckTimeCnt;        //Բ�ȼ��ʱ��
	int  iCheckPos;            //Բ�ȼ��λ��
	int  iSleepTimeCnt;        //�ӹ���ʱ������ʱ�������
	int  iOpLenSum;            //�ӹ��ۼ����
	int  iRepeatCnt;           //�ظ�����
	int  iOpLabelBasePos;      //�ӹ���ײ�����
    int  iOpPageBak;           //�ӹ�ҳ����
}HOLE_ZERO_CTRL;

//�ӹ����Ʊ���
typedef struct  holeCtrl
{
	int iWaitCnt;                            //��е�źŵ��ź�ͬ��
	unsigned char bSynchro;                           //ͬ��λ��	
	HOLE_ZERO_CTRL stZeroCtrl;               //ѭ����0ģ��
}HOLE_CTRL;


class EDM_OP_HOLE : public EDM_OP
{
	typedef unsigned char (EDM_OP_HOLE::*fEdmOpStage)();
public:
	EDM_OP_HOLE(MAC_OPERATE_TYPE enOpType);
	virtual  ~EDM_OP_HOLE();
private:	
    QString m_Cmd;
	HOLE_CTRL m_stOpCtrl;
	DIGIT_CMD m_stOpLabelInit;	
	DIGIT_CMD m_stMvCmd;
	list<fEdmOpStage> m_ListStage;
	list<fEdmOpStage>::iterator m_it;
    MAC_PASS_PARA m_stMacPassPara;
public:
    virtual void EdmOpSetStart(unsigned char bStart);
	virtual void EdmOpCarry();
	virtual void EdmOpOver();
	virtual void EdmOpStageRestart();

protected:
	void SetEdmHolePower(unsigned char bPower,unsigned char bPrune,unsigned char bOterClose);
	void EdmHoleCarry();
	void EdmHoleCmdProcess();

	void EdmHolePauseProcess();
    void EdmHoleOverProcess();
	void EdmHoleMvCmdProcess();
private:
	void SetAllErr();
	unsigned char EdmHoleRise();
	unsigned char EdmHoleUp2Safe();
	unsigned char EdmHoleDownFromSafe();
	unsigned char EdmHoleLocation();
	unsigned char EdmHoleZeroAdjust();
	unsigned char EdmHoleZeroAdjust_Sing();
	unsigned char EdmHolePrune();
	unsigned char EdmHoleOpPage();
	unsigned char EdmHoleMillPage();
	unsigned char EdmHoleRepeat();
	unsigned char EdmHoleRootSleep();
    unsigned char EdmHoleSynchro();

	unsigned char EdmHoleGo2AdjustPos();
	unsigned char EdmHoleReturnOpPos();
	unsigned char EdmHoleGo2StartPos();
    unsigned char EdmHoleResetStartPos();

	void CycleOver();
	void EdmHoleOpTypeInit();
	void EdmHoleRecover();
	void EdmOpGoHigh();
	unsigned char EdmOpMvAheadChkLabel();
	unsigned char EdmOpMvWaitChkLabel();
	unsigned char EdmOpMvBackChkLabel();
	unsigned char ExteedTimeAlarm();
	unsigned char PoleLenAlarm();
	int GetFirstCmdValOfCLabel();
};


