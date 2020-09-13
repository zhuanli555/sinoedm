#pragma once

#include "EDM_OP.h"
#include <list>
using std::list;

//ͨ͸����ģʽ
typedef enum
{
	PASS_NONE = 0,
	PASS_HIGH = 1,
	PASS_LOW  = 2,
}PASS_MODE;

//����ģʽ
typedef enum
{
	CHECK_DIRECT = 0,   //��·ģʽ
	CHECK_ELASTIC = 1,  //����ģʽ
	CHECK_ELEC  = 2,    //���ģʽ
}CHECK_MODE;


typedef struct  isaMAC_OPERATE_PASS_CTRL
{
	PASS_MODE enPassMode;
	int iFeedStd;
	int iVoltageStd;
	int iSpeedStd;
	int iSpeedCalc;
	int iFeedStable[6];
	int iBackStable[6];
	int iVoltageStable[6];
	int iSpeedStable[100];
	int iSpeedRec[50];
	int iVoltageRec[100];
	int iStabelIndex;
	int iSpeedIndex;
	int iSpeedRecIndex;
	int iVoltageRecIndex;
	int iSum;
	int iSumSpeed;
	int iStageCnt;	
	int iVoltage;
	long int iVoltageSum;
	int iPosMax;
	BOOL bStop;
	BOOL bPassLowSet;

	int iFeedEntry;
	BOOL bEntryOver;
	BOOL bEntrySet;
	int iHighTimeCnt;
	int iFiltCnt;

	int iPassRelWork;

	BOOL bPoleSet;
	BOOL bPole;
	BOOL bPoleStable;

	BOOL bHighModeStop;
}MAC_OPERATE_PASS_CTRL;

typedef struct holeZeroCtrl
{
	BOOL bCycleStart;          //�׶ο�ʼ
	BOOL bRepeatSec;           //ÿ���ظ����̵ĵڶ��׶�
	BOOL bWait;                //�ڵȴ���е�źŵ��ź�ͬ��
	BOOL bStageLast;           //ÿһ�׶ε����
	BOOL bMillLast;
	BOOL bEmptyMove;           //��������
	BOOL bMillLenEnough;       //ϳ����ȵ���
	BOOL bMillPause;
	BOOL bSafeLabelExist;
	int  iCheckTimeCnt;        //Բ�ȼ��ʱ��
	int  iCheckPos;            //Բ�ȼ��λ��
	int  iSleepTimeCnt;        //�ӹ���ʱ������ʱ�������
	int  iOpLenSum;            //�ӹ��ۼ����
	int  iRepeatCnt;           //�ظ�����
	int  iOpLabelBasePos;      //�ӹ���ײ�����
	int  iOpPageBak;           //�ӹ�ҳ����
	MAC_OPERATE_PASS_CTRL stPassCtl;//ͨ͸����
}HOLE_ZERO_CTRL;

//�ӹ����Ʊ���
typedef struct  holeCtrl
{
	int iWaitCnt;                            //��е�źŵ��ź�ͬ��
	BOOL bSynchro;                           //ͬ��λ��	
	HOLE_ZERO_CTRL stZeroCtrl;               //ѭ����0ģ��
}HOLE_CTRL;


class EDM_OP_HOLE : public EDM_OP
{
	typedef BOOL (EDM_OP_HOLE::*fEdmOpStage)();
public:
	EDM_OP_HOLE(MAC_OPERATE_TYPE enOpType);
	virtual  ~EDM_OP_HOLE();
private:	
	HOLE_CTRL m_stOpCtrl;
	DIGIT_CMD m_stOpLabelInit;	
	DIGIT_CMD m_stMvCmd;
	list<fEdmOpStage> m_ListStage;
	list<fEdmOpStage>::iterator m_it;
    MAC_PASS_PARA m_stMacPassPara;
public:
	virtual void EdmOpSetStart(BOOL bStart);
	virtual void EdmOpSetTest(BOOL bTest);
	virtual void EdmOpCarry();
	virtual void EdmOpOver();
	virtual void EdmOpStageRestart();
protected:
	void SetEdmHolePower(BOOL bPower,BOOL bPrune,BOOL bOterClose);
	void EdmHoleCarry();
	void EdmHoleCmdProcess();

	void EdmHolePauseProcess();
	void EdmHoleOverProcess();
	void EdmHoleElecProcess();
	void EdmHoleMillFileProcess();
	void EdmHoleMvCmdProcess();
private:
	void SetAllErr();
	BOOL EdmHoleRise();
	BOOL EdmHoleUp2Safe();
	BOOL EdmHoleDownFromSafe();
	BOOL EdmHoleLocation();
	BOOL EdmHoleZeroAdjust();
	BOOL EdmHoleZeroAdjust_Sing();
	BOOL EdmHolePrune();
	BOOL EdmHoleOpPage();
	BOOL EdmHoleMillPage();
	BOOL EdmHoleRepeat();
	BOOL EdmHoleRootSleep();
	BOOL EdmHoleSynchro();
    void EdmHolePassCtl();

	BOOL EdmHoleGo2AdjustPos();
	BOOL EdmHoleReturnOpPos();
	BOOL EdmHoleGo2StartPos();
    BOOL EdmHoleResetStartPos();

	void CycleOver();
	void EdmHoleOpTypeInit();
	void EdmHoleRecover();
	void EdmOpGoHigh();
	BOOL EdmOpMvAheadChkLabel();
	BOOL EdmOpMvWaitChkLabel();
	BOOL EdmOpMvBackChkLabel();
	BOOL ExteedTimeAlarm();
	BOOL PoleLenAlarm();
	int GetFirstCmdValOfCLabel();
};


