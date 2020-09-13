#pragma once

#include "EDM_OP.h"
#include <list>
using std::list;

//通透控制模式
typedef enum
{
	PASS_NONE = 0,
	PASS_HIGH = 1,
	PASS_LOW  = 2,
}PASS_MODE;

//测量模式
typedef enum
{
	CHECK_DIRECT = 0,   //短路模式
	CHECK_ELASTIC = 1,  //伸缩模式
	CHECK_ELEC  = 2,    //光电模式
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
	BOOL bCycleStart;          //阶段开始
	BOOL bRepeatSec;           //每次重复过程的第二阶段
	BOOL bWait;                //在等待机械信号电信号同步
	BOOL bStageLast;           //每一阶段的最后
	BOOL bMillLast;
	BOOL bEmptyMove;           //空走命令
	BOOL bMillLenEnough;       //铣削深度到达
	BOOL bMillPause;
	BOOL bSafeLabelExist;
	int  iCheckTimeCnt;        //圆度检测时间
	int  iCheckPos;            //圆度检测位置
	int  iSleepTimeCnt;        //加工中时间休眠时间计数器
	int  iOpLenSum;            //加工累计深度
	int  iRepeatCnt;           //重复次数
	int  iOpLabelBasePos;      //加工轴底部坐标
	int  iOpPageBak;           //加工页备份
	MAC_OPERATE_PASS_CTRL stPassCtl;//通透控制
}HOLE_ZERO_CTRL;

//加工控制变量
typedef struct  holeCtrl
{
	int iWaitCnt;                            //机械信号电信号同步
	BOOL bSynchro;                           //同步位置	
	HOLE_ZERO_CTRL stZeroCtrl;               //循环置0模块
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


