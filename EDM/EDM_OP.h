#pragma once

#include "EDM.h"
#include "cmdhandle.h"
#include "EDM_OP_File.h"

typedef enum
{
	OP_NO_ERR = 0,             //正常
	OP_HARD_ERR ,              //硬件错误
	OP_EXCEED_TIME ,           //超时限制
	OP_LEN_POLE,               //电极长度不够
}OP_ERROR;

typedef struct opStatusNewCycle
{
	int  iOpPage;              //加工页
	int  iCycleIndex;          //循环编号
	int  iTimeSec;             //时间
	int  iTimeCnt;             //单次加工时间计数器
	unsigned char bPauseCmd;            //暂停指令
    unsigned char bCycleStart;
}STATUS_NEW_CYCYLE;

typedef struct  opStatus
{
	MAC_OPERATE_TYPE enOpType; //加工类型
	STATUS_NEW_CYCYLE stCycle; //状态信息
    OP_ERROR errOp;
    OP_FILE_ERROR errFile;//错误提示
	unsigned char bStart;               //暂停
	unsigned char bOpOver;              //本次加工结束
	unsigned char bCheck_C_Over;        //圆度测量完成
	int  iCmdIndex;            //命令号
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


