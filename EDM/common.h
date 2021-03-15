#ifndef _EDM_H_
#define _EDM_H_

#include <sys/ioctl.h> 

/* 定义设备类型 */
#define IOC_MAGIC  'c'

#define IOC_MSG_VERSION     _IOW(IOC_MAGIC, 1, int)
#define IOC_COMM_INIT           _IOW(IOC_MAGIC, 2, int)
#define IOC_KP_INIT  			     _IOW(IOC_MAGIC, 3, int)
#define IOC_COMM_TO_USER    _IOR(IOC_MAGIC, 4, int)
#define IOC_MOVEPARA_FROM_USER    _IOW(IOC_MAGIC, 5, int)
#define IOC_MAC_OPERATE    _IOW(IOC_MAGIC, 6, int)
#define IOC_SYSTEM_SET    _IOW(IOC_MAGIC, 7, int)
#define IOC_INTERFACE_IN     _IOR(IOC_MAGIC, 8, int)
#define IOC_ROTATE_CTL   _IOW(IOC_MAGIC, 9, int)
#define IOC_PASS_CTL    _IOR(IOC_MAGIC, 10, int)

#define IOC_MAXNR  10

/* 上位机需要 */
#define OPERATE_TRREAD_SLEEP_TIME        20
#define EDM_SHOW_THREAD_SLEEP_TIME       35
#define EDM_MAC_LABEL_COUNT              7
#define EDM_INT_SPEED_MAX      600
#define ARR_EDM_LABEL_SHOW  {0,1,6,4,5,2,3}
#define ARR_EDM_LABEL_SHOW_CHAR {'X','Y','Z','A','B','C','W'}
#define ARR_EDM_LABEL_CHAR {'X','Y','C','W','A','B','Z'}

#define INT_SPEED_CTRL_FREQ_MIN       1000   //速度控制最小频率
#define INT_SPEED_CTRL_ADD            10     //升降速增加值

#define MAC_LABEL_COUNT        7            //轴的总数目,不包括R轴，R轴单独控制
#define MAC_RTZERO_LABEL_COUNT 8
#define OP_HOLE_PAGE_MAX       6
#define MAC_INT_FREQ           10000         //中断频率
#define MAC_LABEL_MOST_LONG    1001          //轴最长,以毫米为单位

#define FALSE 0
#define TRUE  1

#ifdef __cplusplus
extern "C" {
#endif

//车电极
typedef struct edmMacOther
{
	int iLen;
}MAC_OTHER;

typedef struct isaElec_Page 
{
	int iTon;           //脉冲宽度
	int iToff;          //脉宽间歇
	int iElecLow;       //加工电流
	int iElecHigh;      //高压电流
	int iServo;         //伺服给定
	int iFeedSense;     //进给灵敏
	int iBackSense;     //回退灵敏
	int iOpLen;         //加工深度\修整长度
	int iCap;           //加工电容
	int iRotSpeed;      //R轴速度
}Elec_Page;

typedef struct isaElec_Oral
{	
	int iTimeMin;           //最小时限
	int iTimeMax;           //最大时限
	int iRisePos;           //回升位置
	int iSafePos;           //安全位置
	int iJudgePos;          //起判位置
	int iRepeatCount;       //重复次数
	int iRepeatLen;         //重复长度
	int iBottomSleep;       //底部停歇
	int iMillServo;         //铣削伺服
	int iOpLenAll;          //总深度
	int iOpHoleIndex;       //加工孔号
	int iOpHoleAll;         //加工总孔数
	unsigned long bRotateValidate;   //旋转有效
	unsigned long bContinueOp;       //继续加工
}Elec_Oral;

//电加工参数
typedef struct  isaMAC_ELEC_PARA
{
	Elec_Page stElecPage[OP_HOLE_PAGE_MAX];
	Elec_Oral stElecOral;
	int iParaIndex;
}MAC_ELEC_PARA;


//旋转参数
typedef struct isaMAC_ROTATE_PARA
{
	unsigned long bRotate;
	unsigned long bDir;
	int iFreq;
}MAC_ROTATE_PARA;

/**
 * 加工类别
 * OP_TYPE_NONE       = 0,    //无加工状态
	//小孔
	OP_HOLE_SING       = 1,    //单孔加工
	OP_HOLE_PROGRAME   = 2,    //程序加工
	OP_HOLE_SIMULATE   = 3,    //模拟加工
	OP_HOLE_MILL       = 4,    //铣削加工
	OP_HOLE_CHECK_C    = 5,    //圆度测量
	OP_HOLE_DISCHARGE  = 9,    //对刀
	OP_MOLD_GRIND      = 11,   //磨削加工
	//成型
	OP_MOLD_SHAPE      = 21,   //成型加工	
*/
typedef enum
{
	OP_TYPE_NONE       = 0,    //无加工状态
	//小孔
	OP_HOLE_SING       = 1,    //单孔加工
	OP_HOLE_PROGRAME   = 2,    //程序加工
	OP_HOLE_SIMULATE   = 3,    //模拟加工
	OP_HOLE_MILL       = 4,    //铣削加工
	OP_HOLE_CHECK_C    = 5,    //圆度测量
	OP_HOLE_DISCHARGE  = 9,    //对刀

	OP_MOLD_GRIND      = 11,   //磨削加工
	//成型
	OP_MOLD_SHAPE      = 21,   //成型加工	
}MAC_OPERATE_TYPE;

/**
 * 轴运动方式
 * 0 运动物理结束
	1 运动中
	2 回零运动
	3 正常运动补偿
*/
typedef enum
{
	RULE_MOVE_OVER      = 0,     //运动物理结束
	RULE_IN_MOVE        = 1,     //运动中
	RULE_RTZERO         = 2,     //回零运动
	RULE_COMPEN         = 3,     //正常运动补偿
}MOVE_STATUS;

/**
 * 轴运动问题
 * 0 没有问题
	1 软限位问题
	2 硬限位问题
	3 非法使用
	4 回零错误
	5 程序编制错误
*/
typedef enum
{
	MOVE_NO_TROUBLE     = 0,    //没有问题
	MOVE_SOFT_TROUBLE   = 1,    //软限位问题
	MOVE_HARD_TROUBLE   = 2,    //硬限位问题
	MOVE_UN_LAW         = 3,    //非法使用
	MOVE_RT_ZERO_ERR    = 4,    //回零错误
	MOVE_PROG_ERR       = 5,    //程序编制错误
}MOVE_TROUBLE;

typedef enum
{
	COOR_G54 = 0,
	COOR_G55,
	COOR_G56,
	COOR_G57,
	COOR_G58,
	COOR_G59,
}EDM_COOR_TYPE;

/**
 * 运动目标类型
 * 0 绝对尺寸
 * 1 增量尺寸
 * 2 工件坐标原点设置
*/
typedef enum
{
	AIM_G90 = 0,//绝对尺寸
	AIM_G91 = 1,//增量尺寸
	AIM_G92 = 2,	//工件坐标原点设置
}EDM_AIM_TYPE;

/**
 * 运动轨迹类型
 * 0 快速运动
 * 1 直线插补
 * 2 顺圆
 * 3 逆圆
*/
typedef enum
{
	ORBIT_G00 = 0,      //快速运动
	ORBIT_G01 = 1,      //直线插补
	ORBIT_G02 = 2,      //顺圆
	ORBIT_G03 = 3,      //逆圆
}EDM_ORBIT_TYPE;


/**
 * 轴运动使用的齿补控制参数
 * iChiBu 齿补数，从数据库中读
 * */
typedef struct  kpAxis_ChiBuVal
{
	int iChiBu;                                //齿补数，从数据库中读
}Axis_ChiBuVal;

//轴运动使用的螺补控制参数
typedef struct  kpAxis_LuoBuVal
{
	int  iLuobuPoint[MAC_LABEL_MOST_LONG];     //螺补表
}Axis_LuoBuVal;

//轴运动使用的控制参数硬件参数
typedef struct  kpAxis_Hard_Para
{	
	unsigned long bPosLimit;              //正限位
	unsigned long bNegLimit;              //负限位
	unsigned long bAlarm;                 //轴报警
}Axis_Hard_Para;

//轴运动中用户设置的控制参数软件参数
typedef struct  kpAxis_Soft_Para
{
	char cSymbol;                //轴标志
	unsigned long bUse;                   //标示使用某一轴,从数据库中读
	unsigned long bLimitNoUse;            //标示限位开关不使用，A/C可以设置，其余都必须使用
	unsigned long bRotateLabel;           //是否旋转轴
	unsigned long bPosMonitor;            //光栅尺监测标志
	unsigned long bDirectMotor;           //直驱电机
	int iLowerPos;               //软限位下值
	int iTopPos;                 //软限位上值
	int iZeroMakeUp;             //零位补偿
	int iRasterLinear;           //光栅尺系数
	int iPosPrecision;           //光栅尺误差
	int iLocateArea;             //所属区域,0,1,2..
}Axis_Soft_Para;

//轴运动使用的控制参数
typedef struct  kpAxis_Para_Kp
{
	Axis_Soft_Para    stSoftPara;
	Axis_ChiBuVal     stChiBuVal;
	Axis_LuoBuVal     stLuoBuVal;
}Axis_Para_Kp;

/**
 * 轴运动KP\USER都使用的参数
 * int iMachPos;           机械位置，从内存中读
	int iWorkPosSet;        工作坐标设定对应机械位置值
	int iRasilPos;          光栅尺坐标
	unsigned long bDirMove;          新的运动方向
	unsigned long dwPulseCount;     调试用,发送脉冲数
*/
typedef struct isaAxis_Para_Common
{
	int iMachPos;           //机械位置，从内存中读
	int iWorkPosSet;        //工作坐标设定对应机械位置值
	int iRasilPos;          //光栅尺坐标
	unsigned long bDirMove;          //新的运动方向
	unsigned long dwPulseCount;     //调试用,发送脉冲数
}Axis_Para_Common;

typedef struct isaMac_Pass_Para
{
	unsigned long bOver;
	int iFeedCnt;
	int iBackCnt;
}MAC_PASS_PARA;

//加工过程参数
typedef struct isaEdm_OP
{
	MAC_OPERATE_TYPE enOpType;    //加工方式	
	unsigned long bOpenPower;              //开电源
	unsigned long bSetInitPos;             //设置起始位置
	unsigned long bShortDis;         
	int iFeedSense;               //进给灵敏
	int iBackSense;               //回退灵敏
	int iMillSense;               //伺服灵敏
}Edm_OP;

typedef struct isaMAC_COMMON
{
	Axis_Para_Common stMoveCtrlComm[MAC_LABEL_COUNT];
	//Mac_Pass_Para    stPass;      //穿透控制
	int  iPosRAxis;                //R轴位置
	unsigned long bDirect;                  //短路标志
	unsigned long bPowerBack;               //电加工回退
	MOVE_STATUS enMvStatus;        //运动方式
	MOVE_TROUBLE enMvTrouble;      //运动出现问题
	MAC_OPERATE_TYPE enOpType;     //加工类别
}MAC_COMMON;

/**
 * 加工参数
 * Axis_Para_Kp  stAxisCtrlKp[MAC_LABEL_COUNT] 轴运动使用的参数
	int  iOpLabel                    加工轴
	unsigned long bOpDir 加工方向
	unsigned long bDebug 代码调试参数，软参数
	unsigned long bNoCheck 需要用检测信号
*/
typedef struct isaMAC_KPINT
{
	Axis_Para_Kp  stAxisCtrlKp[MAC_LABEL_COUNT];
	int  iOpLabel;                    //加工轴
	unsigned long bOpDir;                      //加工方向
	unsigned long bDebug;                      //代码调试参数，软参数
	unsigned long bNoCheck;                    //需要用检测信号
}MAC_KPINT;//驱动内部执行变量

typedef struct kpMAC_SPEED_CTL 
{
	int iFreqSpeed;
	int iSpeedAscendCnt;
	int iHandBoxLabel;
	unsigned long bHandBox;
	unsigned long bHandBoxDir;
	unsigned long bAscendStart;
	unsigned long bDescendStart;
}MAC_SPEED_CTL;

typedef struct isaAxisDigitPara
{
	int iLabel;      //轴标志
	int iDistance;   //运动距离
}AxisDigitPara;

/**
 * int  iFreq 速度对应频率
	int  iAxisCnt 进入驱动轴数
	unsigned long bNoCheck 需要用检测信号
	EDM_AIM_TYPE enAim 命令类型
	EDM_ORBIT_TYPE enOrbit 轨迹类型
	EDM_COOR_TYPE  enCoor 多坐标系
	Edm_OP stOp 加工参数	
	AxisDigitPara stAxisDigit[15] 轴的运动参数,最多一次送10个轴
*/
typedef struct edmDIGIT_CMD
{
	int  iFreq;                     //速度对应频率
	int  iAxisCnt;                  //进入驱动轴数
	unsigned long bNoCheck;                  //需要用检测信号
	EDM_AIM_TYPE enAim;             //命令类型
	EDM_ORBIT_TYPE enOrbit;         //轨迹类型
	EDM_COOR_TYPE  enCoor;          //多坐标系
	Edm_OP stOp;                    //加工参数	
	AxisDigitPara stAxisDigit[15];  //轴的运动参数,最多一次送10个轴
}DIGIT_CMD;

typedef struct edmDIGIT_CMD_VECT
{
	int iAxisCnt;
	AxisDigitPara stAxisDigit[6];  //矢量轴的运动参数
}DIGIT_CMD_VECT;

typedef struct isaMAC_HANDLE_ENTILE
{
	unsigned long bStop;                     //总停
	unsigned long bNoProtect;                //防撞保护取反
	unsigned long bRtZero;                   //回零
	int  iLabel;                    //回零轴
}MAC_HANDLE_ENTILE;//手动控制

typedef struct isaMAC_INTERFACE_IN
{
    unsigned char btI140;
    unsigned char btI144;
    unsigned char btI148;
    unsigned char btI184;
    unsigned char btI188;
    unsigned char btI1C0;
    unsigned char btI1C4;
    unsigned char btI1C8;
}MAC_INTERFACE_IN;

typedef struct  isaMAC_INTERFACE_OUT
{
    unsigned char btO140;//x,y,z
    unsigned char btO144;
	unsigned char btO184;
	unsigned char btO188;
    unsigned char btO18C;//中断
	unsigned char btO190;
    unsigned char btO198;
    unsigned char btO199;
    unsigned char btO1C0;
    unsigned char btO1C4;

}MAC_INTERFACE_OUT;

typedef struct isaMAC_SYSTEM_SET_NONE_LABEL
{
	unsigned long bWaterMonitor;
	unsigned long bTempteratureMonitor;
	unsigned long bPowerMonitor;
	unsigned long bLowPumpUnion;
	unsigned long bHighPumpUnion;
	unsigned long bPass;
	unsigned long bCycleMeasure;
	unsigned long bOpDir;
	unsigned long bDebug;
	int  iLabel_C_xMachine;
	int  iLabel_C_yMachine;
	int  iLabel_S_Length;
	int  iPassLen;
	int  iRAxisSpeed;
	int  iSifu;
	int  iToff;
	int  iCap;
	int  iRotate;
	int  iPoleMode;	
	int iOpLabel;
	int iPoleBoxWidth;
	int iWholeSpeed;
	unsigned long bHandWheel;
	int iNLabelLinear;
	int iLabel_C_z_Dvalue;
	int iLabel_A_C_Center;
	int iLabel_A_Distance;
	int iLabelWorkAbs_X;
	int iLabelWorkAbs_Y;
	int iTime;
	int iTimeOp;
	unsigned long bBlindAble;
	int iBlindPos_X;
	int iBlindPos_Y;
	int iBlind_Deviate;
}Mac_System_Set_None_Label;//系统设置时不涉及轴

typedef struct isaMAC_SYSTEM_SET
{	
	int iSetChiBu[MAC_LABEL_COUNT];
	unsigned long bSetUse[MAC_LABEL_COUNT];
	unsigned long bSetLimitNoUse[MAC_LABEL_COUNT];
	int iSetZeroMakeUp[MAC_LABEL_COUNT];
	unsigned long bPosMonitor[MAC_LABEL_COUNT];
	int  iRasterLinear[MAC_LABEL_COUNT];
	int iPosPrecision[MAC_LABEL_COUNT];
	int iAxistLabel;
	Mac_System_Set_None_Label stSetNoneLabel;
}MAC_SYSTEM_SET;//系统设置

unsigned long CalcLimitBool(MAC_INTERFACE_IN *pIn,int iLabel,unsigned long bDir,unsigned long bDirectMotor);
unsigned long CalcAlarmBool(MAC_INTERFACE_IN *pIn,int iLabel);
unsigned long CalcDirectBool(MAC_INTERFACE_IN *pIn);

#ifdef __cplusplus
}
#endif

#endif
