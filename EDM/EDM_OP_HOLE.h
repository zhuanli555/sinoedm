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
	unsigned char bStop;
	unsigned char bPassLowSet;

	int iFeedEntry;
	unsigned char bEntryOver;
	unsigned char bEntrySet;
	int iHighTimeCnt;
	int iFiltCnt;

	int iPassRelWork;

	unsigned char bPoleSet;
	unsigned char bPole;
	unsigned char bPoleStable;

	unsigned char bHighModeStop;
}MAC_OPERATE_PASS_CTRL;

typedef struct holeZeroCtrl
{
	unsigned char bCycleStart;          //阶段开始
	unsigned char bRepeatSec;           //每次重复过程的第二阶段
	unsigned char bWait;                //在等待机械信号电信号同步
	unsigned char bStageLast;           //每一阶段的最后
	unsigned char bMillLast;
	unsigned char bEmptyMove;           //空走命令
	unsigned char bMillLenEnough;       //铣削深度到达
	unsigned char bMillPause;
	unsigned char bSafeLabelExist;
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
	unsigned char bSynchro;                           //同步位置	
	HOLE_ZERO_CTRL stZeroCtrl;               //循环置0模块
}HOLE_CTRL;


class EDM_OP_HOLE : public EDM_OP
{
	typedef unsigned char (EDM_OP_HOLE::*fEdmOpStage)();
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
	virtual void EdmOpSetStart(unsigned char bStart);
	virtual void EdmOpSetTest(unsigned char bTest);
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
    void EdmHolePassCtl();

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


