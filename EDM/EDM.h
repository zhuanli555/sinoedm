#ifndef EDM_H
#define EDM_H

#include <QObject>
#include "common.h"
#include "EDM_Db.h"
#include "electool.h"
#include "cmdhandle.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <map>
#include <vector>

using namespace std;

typedef enum
{
	MSG_PUMP=1,
	MSG_ROTATE,
	MSG_STOP,
	MSG_RTZERO,
	MSG_PROTECT,
	MSG_TRANSFER,
	MSG_PRUNE,
	MSG_RTCENTER_C,
	MSG_MAC_PARA_SET,
	MSG_AXIS_ADJUST,
	MSG_VECT_WORK_SET,
	MSG_VECT_C_CENTER,
	MSG_ADJUST_CIRCLE,
}MSG_MOVE;

typedef enum
{
	MSG_OP =0,
	MSG_PAUSE,
	MSG_FILE,
	MSG_ELEC,
	MSG_TEST,
	MSG_TEST_C,
	MSG_OP_OVER,
}MSG_TYPE;

//轴运动使用的控制参数硬件参数
typedef struct  edmAxis_Adjust
{	
	unsigned char bAdjust;
	unsigned char bDir;
	unsigned char bOver;
	int iLabel;
	int iStage;
	int iAdjustCnt;
	int iPos;
}Axis_Adjust;

//内外圆基准控制
typedef enum
{
	EN_ADJUST_NONE          = 0,     //没有内外圆基准
	EN_ADJUST_INSIDE        = 1,     //内圆基准
	EN_ADJUST_OUTSIDE       = 2,     //外圆基准
}ADJUST_TYPE;

typedef struct edmAdjustCircle 
{
	ADJUST_TYPE enAdjustType;
	int iIndex;
	int iIndexCnt;
	int iHeight;
	int iSemiDiameter;
	int iVal_X[5];
	int iVal_Y[5];
	unsigned char bAdjust;
	unsigned char bOver;
}ADJUST_CIRCLE;


//轴运动使用的控制参数硬件参数
typedef struct  edmAxis_Hard_Ctl
{	
	bool bPosLimit;              //正限位
	bool bNegLimit;              //负限位
	bool bAlarm;                 //轴报警
}Axis_Hard_Ctl;

typedef struct  edmMove_Hard_Para
{
	Axis_Hard_Ctl stHardCtlUser[MAC_LABEL_COUNT];
	bool           bDirect;
}Move_Hard_Para;

typedef struct  edmMac_Status
{
	bool bStop;       //总停
	bool bPumpLow;    //低压泵
	bool bPumpHigh;   //高压泵
    bool bShake;      //振动
	bool bPower;      //高频
	bool bPrune;      //修电极
	bool bDirect;     //短路
	bool bRotate;     //旋转
	bool bLowPressure;//欠水压
	bool bRalarm;    //R轴报警
	bool bLubricate;  //润滑报警
	bool bAxisOffset; //轴偏差
	bool bKeyShut;    //键锁定
	bool bNoProtect;  //防撞保护取反
}Mac_Status;

typedef struct edmEDM_ALL_DATA
{
	MAC_COMMON stComm;
	int iWorkPos[MAC_LABEL_COUNT];
	Move_Hard_Para stHardCtl;
	Mac_Status stStatus;
	MAC_INTERFACE_IN stEdmInterfaceIn;
	MAC_INTERFACE_OUT stEdmInterfaceOut;
	EDM_COOR_TYPE enCoorType;
}EDM_ALL_DATA;

class EDM : public QObject
{
    Q_OBJECT

private:
    explicit EDM(QObject *parent = nullptr);
    static EDM* m_pEdm;
	virtual ~EDM();
public:
	static EDM* GetEdmInstance();
	static void DelEdm();	
public:	
    static QString m_strElecDefault;
	MAC_COMMON m_stEdmComm;
	EDM_ALL_DATA m_stEdmShowData;	
	MAC_KPINT m_stEdmKpInt;
	MAC_SYSTEM_SET m_stSysSet;
	int m_iWorkIndex;
	bool m_bMachFault;

    QString m_strSysPath;
    map<int,int> m_mpMakeUp_C;
private:
    int fd;
	MAC_INTERFACE_OUT m_stEdmInterfaceOut;
	MAC_INTERFACE_IN m_stEdmInterfaceIn;	
	Mac_Status m_stStatus;
    MAC_HANDLE_ENTILE m_stEdmOpEntile;
	
	int m_iCoor[MAC_LABEL_COUNT][6];
    QString m_strOpName;
	MAC_OTHER m_stMacOther;
    EDM_Db * m_pEdmAdoSys;
    MAP_ELEC_MAN  mp_ElecMan;
	Axis_Adjust m_stAxisAdjust;
	ADJUST_CIRCLE m_stAdjustCircle;

    QString m_strMakeUpFile;
public:
	bool EdmInit();
	bool EdmClose();
	void GetMacPara(MAC_SYSTEM_SET* pSysSet);
	bool GetEdmComm();
	bool GetEdmMacPassPara(MAC_PASS_PARA* pPass);
	int  GetEdmAxisWorkPos(int iLabel);
	bool GetEdmStatusData();

	int  EdmHandProcess();

	int GetRAxisFreq(int iSpeed);
	
	void CloseHardWare();

	bool EdmStopMove(bool bStatus);
	void EdmStopSignClose();
	void EdmZeroSignClose();
	void EdmSaveMacComm();

	void SetAxisAdjust(int iLabel, unsigned char bDir);
	void SetAxisAdjustCircleType(ADJUST_CIRCLE* pAdjustCircle);
	void EdmAxisAdjust();
    void EdmAxisAdjustCircle();

	void SetValMakeUp(int iVal_C,int iVal_X);
    void SetMakeUpFile(QString strFile);
	void ClearMakeUpVal();
	void AutoClearMakeUpVal(DIGIT_CMD* pMacUser);
	bool HasMakeUpVal();
    bool GetMakeUpVal(int iVal_C,int* pVal);

private:
	bool EdmRedLump(bool bRed);
	void EdmReadMacPara();
	int  WriteByteToReg(int iCtrAdd,unsigned char btCtrVal);
	unsigned char ReadByteFromReg(int iCtrAdd);
	void SetServoToGive(int iPercent);
    bool GetAxisOffset();
	int GetSpeed(int iFreq);
	int HandBoxProcess();	
	bool SwitchWorkIndex(int iIndex);
	void RetCenter_C_G59();
	void EdmAxisAdjustCircleInside();
	void EdmAxisAdjustCircleOutSide();
public:
    unsigned char FindElecManElem(QString str);
    void GetElecManElem(QString str,MAC_ELEC_PARA* pElecMan);
    int WriteElecPara(Elec_Page *pElecPara,QString strFunc);
	bool GetWorkPosSetByIndex(int iIndex,int iWork[]);
	void ReSetWorkPosSetByIndex(int iIndex,int iWork[]);
public:
	bool SaveMacPara(MAC_SYSTEM_SET* pSysSet);
    void test_inter();
public slots:
	bool EdmSetProtect(bool bProtect);
	bool EdmLowPump(bool bOpen);
	bool EdmHighPump(bool bOpen);
    bool EdmSetShake(bool bShake);
	bool EdmRotate(bool bSwitch,bool bOpen,int iSpeed,bool bDir);
	bool EdmPower(bool bOpen);
	bool EdmPrune(bool bOpen);
	bool EdmHummer(bool bOpen);	
	bool EdmYellowLump(bool bYellow);
    void EdmStop();
    bool EdmRtZero(int iLabel);
    bool EdmSendMovePara(DIGIT_CMD* pMacUser);
    void EdmRtCenterC();
    void EdmSetVectWorkPos();
    void EdmSetVectCenter_CPos();

};
#endif // EDM_H
