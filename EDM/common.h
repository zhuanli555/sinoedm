#ifndef _EDM_H_
#define _EDM_H_

#include <sys/ioctl.h> 

/* �����豸���� */
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

/* ��λ����Ҫ */
#define OPERATE_TRREAD_SLEEP_TIME        20
#define EDM_SHOW_THREAD_SLEEP_TIME       35
#define EDM_MAC_LABEL_COUNT              7
#define EDM_INT_SPEED_MAX      600
#define ARR_EDM_LABEL_SHOW  {0,1,6,4,5,2,3}
#define ARR_EDM_LABEL_SHOW_CHAR {'X','Y','Z','A','B','C','W'}
#define ARR_EDM_LABEL_CHAR {'X','Y','C','W','A','B','Z'}

#define INT_SPEED_CTRL_FREQ_MIN       1000   //�ٶȿ�����СƵ��
#define INT_SPEED_CTRL_ADD            10     //����������ֵ

#define MAC_LABEL_COUNT        7            //�������Ŀ,������R�ᣬR�ᵥ������
#define MAC_RTZERO_LABEL_COUNT 8
#define OP_HOLE_PAGE_MAX       6
#define MAC_INT_FREQ           10000         //�ж�Ƶ��
#define MAC_LABEL_MOST_LONG    1001          //���,�Ժ���Ϊ��λ

#define FALSE 0
#define TRUE  1

#ifdef __cplusplus
extern "C" {
#endif

//���缫
typedef struct edmMacOther
{
	int iLen;
}MAC_OTHER;

typedef struct isaElec_Page 
{
	int iTon;           //������
	int iToff;          //�����Ъ
	int iElecLow;       //�ӹ�����
	int iElecHigh;      //��ѹ����
	int iServo;         //�ŷ�����
	int iFeedSense;     //��������
	int iBackSense;     //��������
	int iOpLen;         //�ӹ����\��������
	int iCap;           //�ӹ�����
	int iRotSpeed;      //R���ٶ�
}Elec_Page;

typedef struct isaElec_Oral
{	
	int iTimeMin;           //��Сʱ��
	int iTimeMax;           //���ʱ��
	int iRisePos;           //����λ��
	int iSafePos;           //��ȫλ��
	int iJudgePos;          //����λ��
	int iRepeatCount;       //�ظ�����
	int iRepeatLen;         //�ظ�����
	int iBottomSleep;       //�ײ�ͣЪ
	int iMillServo;         //ϳ���ŷ�
	int iOpLenAll;          //�����
	int iOpHoleIndex;       //�ӹ��׺�
	int iOpHoleAll;         //�ӹ��ܿ���
	unsigned long bRotateValidate;   //��ת��Ч
	unsigned long bContinueOp;       //�����ӹ�
}Elec_Oral;

//��ӹ�����
typedef struct  isaMAC_ELEC_PARA
{
	Elec_Page stElecPage[OP_HOLE_PAGE_MAX];
	Elec_Oral stElecOral;
	int iParaIndex;
}MAC_ELEC_PARA;


//��ת����
typedef struct isaMAC_ROTATE_PARA
{
	unsigned long bRotate;
	unsigned long bDir;
	int iFreq;
}MAC_ROTATE_PARA;

/**
 * �ӹ����
 * OP_TYPE_NONE       = 0,    //�޼ӹ�״̬
	//С��
	OP_HOLE_SING       = 1,    //���׼ӹ�
	OP_HOLE_PROGRAME   = 2,    //����ӹ�
	OP_HOLE_SIMULATE   = 3,    //ģ��ӹ�
	OP_HOLE_MILL       = 4,    //ϳ���ӹ�
	OP_HOLE_CHECK_C    = 5,    //Բ�Ȳ���
	OP_HOLE_DISCHARGE  = 9,    //�Ե�
	OP_MOLD_GRIND      = 11,   //ĥ���ӹ�
	//����
	OP_MOLD_SHAPE      = 21,   //���ͼӹ�	
*/
typedef enum
{
	OP_TYPE_NONE       = 0,    //�޼ӹ�״̬
	//С��
	OP_HOLE_SING       = 1,    //���׼ӹ�
	OP_HOLE_PROGRAME   = 2,    //����ӹ�
	OP_HOLE_SIMULATE   = 3,    //ģ��ӹ�
	OP_HOLE_MILL       = 4,    //ϳ���ӹ�
	OP_HOLE_CHECK_C    = 5,    //Բ�Ȳ���
	OP_HOLE_DISCHARGE  = 9,    //�Ե�

	OP_MOLD_GRIND      = 11,   //ĥ���ӹ�
	//����
	OP_MOLD_SHAPE      = 21,   //���ͼӹ�	
}MAC_OPERATE_TYPE;

/**
 * ���˶���ʽ
 * 0 �˶��������
	1 �˶���
	2 �����˶�
	3 �����˶�����
*/
typedef enum
{
	RULE_MOVE_OVER      = 0,     //�˶��������
	RULE_IN_MOVE        = 1,     //�˶���
	RULE_RTZERO         = 2,     //�����˶�
	RULE_COMPEN         = 3,     //�����˶�����
}MOVE_STATUS;

/**
 * ���˶�����
 * 0 û������
	1 ����λ����
	2 Ӳ��λ����
	3 �Ƿ�ʹ��
	4 �������
	5 ������ƴ���
*/
typedef enum
{
	MOVE_NO_TROUBLE     = 0,    //û������
	MOVE_SOFT_TROUBLE   = 1,    //����λ����
	MOVE_HARD_TROUBLE   = 2,    //Ӳ��λ����
	MOVE_UN_LAW         = 3,    //�Ƿ�ʹ��
	MOVE_RT_ZERO_ERR    = 4,    //�������
	MOVE_PROG_ERR       = 5,    //������ƴ���
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
 * �˶�Ŀ������
 * 0 ���Գߴ�
 * 1 �����ߴ�
 * 2 ��������ԭ������
*/
typedef enum
{
	AIM_G90 = 0,//���Գߴ�
	AIM_G91 = 1,//�����ߴ�
	AIM_G92 = 2,	//��������ԭ������
}EDM_AIM_TYPE;

/**
 * �˶��켣����
 * 0 �����˶�
 * 1 ֱ�߲岹
 * 2 ˳Բ
 * 3 ��Բ
*/
typedef enum
{
	ORBIT_G00 = 0,      //�����˶�
	ORBIT_G01 = 1,      //ֱ�߲岹
	ORBIT_G02 = 2,      //˳Բ
	ORBIT_G03 = 3,      //��Բ
}EDM_ORBIT_TYPE;


/**
 * ���˶�ʹ�õĳݲ����Ʋ���
 * iChiBu �ݲ����������ݿ��ж�
 * */
typedef struct  kpAxis_ChiBuVal
{
	int iChiBu;                                //�ݲ����������ݿ��ж�
}Axis_ChiBuVal;

//���˶�ʹ�õ��ݲ����Ʋ���
typedef struct  kpAxis_LuoBuVal
{
	int  iLuobuPoint[MAC_LABEL_MOST_LONG];     //�ݲ���
}Axis_LuoBuVal;

//���˶�ʹ�õĿ��Ʋ���Ӳ������
typedef struct  kpAxis_Hard_Para
{	
	unsigned long bPosLimit;              //����λ
	unsigned long bNegLimit;              //����λ
	unsigned long bAlarm;                 //�ᱨ��
}Axis_Hard_Para;

//���˶����û����õĿ��Ʋ����������
typedef struct  kpAxis_Soft_Para
{
	char cSymbol;                //���־
	unsigned long bUse;                   //��ʾʹ��ĳһ��,�����ݿ��ж�
	unsigned long bLimitNoUse;            //��ʾ��λ���ز�ʹ�ã�A/C�������ã����඼����ʹ��
	unsigned long bRotateLabel;           //�Ƿ���ת��
	unsigned long bPosMonitor;            //��դ�߼���־
	unsigned long bDirectMotor;           //ֱ�����
	int iLowerPos;               //����λ��ֵ
	int iTopPos;                 //����λ��ֵ
	int iZeroMakeUp;             //��λ����
	int iRasterLinear;           //��դ��ϵ��
	int iPosPrecision;           //��դ�����
	int iLocateArea;             //��������,0,1,2..
}Axis_Soft_Para;

//���˶�ʹ�õĿ��Ʋ���
typedef struct  kpAxis_Para_Kp
{
	Axis_Soft_Para    stSoftPara;
	Axis_ChiBuVal     stChiBuVal;
	Axis_LuoBuVal     stLuoBuVal;
}Axis_Para_Kp;

/**
 * ���˶�KP\USER��ʹ�õĲ���
 * int iMachPos;           ��еλ�ã����ڴ��ж�
	int iWorkPosSet;        ���������趨��Ӧ��еλ��ֵ
	int iRasilPos;          ��դ������
	unsigned long bDirMove;          �µ��˶�����
	unsigned long dwPulseCount;     ������,����������
*/
typedef struct isaAxis_Para_Common
{
	int iMachPos;           //��еλ�ã����ڴ��ж�
	int iWorkPosSet;        //���������趨��Ӧ��еλ��ֵ
	int iRasilPos;          //��դ������
	unsigned long bDirMove;          //�µ��˶�����
	unsigned long dwPulseCount;     //������,����������
}Axis_Para_Common;

typedef struct isaMac_Pass_Para
{
	unsigned long bOver;
	int iFeedCnt;
	int iBackCnt;
}MAC_PASS_PARA;

//�ӹ����̲���
typedef struct isaEdm_OP
{
	MAC_OPERATE_TYPE enOpType;    //�ӹ���ʽ	
	unsigned long bOpenPower;              //����Դ
	unsigned long bSetInitPos;             //������ʼλ��
	unsigned long bShortDis;         
	int iFeedSense;               //��������
	int iBackSense;               //��������
	int iMillSense;               //�ŷ�����
}Edm_OP;

typedef struct isaMAC_COMMON
{
	Axis_Para_Common stMoveCtrlComm[MAC_LABEL_COUNT];
	//Mac_Pass_Para    stPass;      //��͸����
	int  iPosRAxis;                //R��λ��
	unsigned long bDirect;                  //��·��־
	unsigned long bPowerBack;               //��ӹ�����
	MOVE_STATUS enMvStatus;        //�˶���ʽ
	MOVE_TROUBLE enMvTrouble;      //�˶���������
	MAC_OPERATE_TYPE enOpType;     //�ӹ����
}MAC_COMMON;

/**
 * �ӹ�����
 * Axis_Para_Kp  stAxisCtrlKp[MAC_LABEL_COUNT] ���˶�ʹ�õĲ���
	int  iOpLabel                    �ӹ���
	unsigned long bOpDir �ӹ�����
	unsigned long bDebug ������Բ����������
	unsigned long bNoCheck ��Ҫ�ü���ź�
*/
typedef struct isaMAC_KPINT
{
	Axis_Para_Kp  stAxisCtrlKp[MAC_LABEL_COUNT];
	int  iOpLabel;                    //�ӹ���
	unsigned long bOpDir;                      //�ӹ�����
	unsigned long bDebug;                      //������Բ����������
	unsigned long bNoCheck;                    //��Ҫ�ü���ź�
}MAC_KPINT;//�����ڲ�ִ�б���

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
	int iLabel;      //���־
	int iDistance;   //�˶�����
}AxisDigitPara;

/**
 * int  iFreq �ٶȶ�ӦƵ��
	int  iAxisCnt ������������
	unsigned long bNoCheck ��Ҫ�ü���ź�
	EDM_AIM_TYPE enAim ��������
	EDM_ORBIT_TYPE enOrbit �켣����
	EDM_COOR_TYPE  enCoor ������ϵ
	Edm_OP stOp �ӹ�����	
	AxisDigitPara stAxisDigit[15] ����˶�����,���һ����10����
*/
typedef struct edmDIGIT_CMD
{
	int  iFreq;                     //�ٶȶ�ӦƵ��
	int  iAxisCnt;                  //������������
	unsigned long bNoCheck;                  //��Ҫ�ü���ź�
	EDM_AIM_TYPE enAim;             //��������
	EDM_ORBIT_TYPE enOrbit;         //�켣����
	EDM_COOR_TYPE  enCoor;          //������ϵ
	Edm_OP stOp;                    //�ӹ�����	
	AxisDigitPara stAxisDigit[15];  //����˶�����,���һ����10����
}DIGIT_CMD;

typedef struct edmDIGIT_CMD_VECT
{
	int iAxisCnt;
	AxisDigitPara stAxisDigit[6];  //ʸ������˶�����
}DIGIT_CMD_VECT;

typedef struct isaMAC_HANDLE_ENTILE
{
	unsigned long bStop;                     //��ͣ
	unsigned long bNoProtect;                //��ײ����ȡ��
	unsigned long bRtZero;                   //����
	int  iLabel;                    //������
}MAC_HANDLE_ENTILE;//�ֶ�����

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
    unsigned char btO18C;//�ж�
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
}Mac_System_Set_None_Label;//ϵͳ����ʱ���漰��

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
}MAC_SYSTEM_SET;//ϵͳ����

unsigned long CalcLimitBool(MAC_INTERFACE_IN *pIn,int iLabel,unsigned long bDir,unsigned long bDirectMotor);
unsigned long CalcAlarmBool(MAC_INTERFACE_IN *pIn,int iLabel);
unsigned long CalcDirectBool(MAC_INTERFACE_IN *pIn);

#ifdef __cplusplus
}
#endif

#endif
