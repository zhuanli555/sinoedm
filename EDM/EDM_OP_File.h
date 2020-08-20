#pragma once

#include "EDM.h"
#include "cmdhandle.h"

typedef enum
{
	OP_FILE_NO_ERR = 0,             //正常
	OP_FILE_NO_EXIST,
	OP_FILE_NO_CMD,
	OP_FILE_NO_ELEC,
	OP_FILE_EDIT_ERR,
	OP_FILE_ERR_ELEC,
}OP_FILE_ERROR;

typedef map<QString ,MAP_ELEC_MAN>  MAP_ELEC_MAN_ALL;

class EDM_OP_File
{
public:
	EDM_OP_File();
	virtual  ~EDM_OP_File();
public:
    vector<QString> m_vCmd;         //加工程序
    vector<QString> m_vCmdStd;      //标准的程序
    vector<QString> m_vCmdLoc;
	MAP_ELEC_MAN m_mpElecMan;       //电参数管理	
    QString m_strElec;              //当前电单数名称
	OP_FILE_ERROR m_enOpFileErr;    //加工文件有错误

	int m_iCmdNum;                  //总的数目
	QString m_sFile;	            //文件名称
	QString m_sPath;              //路径

	EDM_COOR_TYPE  m_enCoor;
protected:
	EDM* m_pEdm;	

	EDM_AIM_TYPE m_enAim;           //运动目标类型
	EDM_ORBIT_TYPE m_enOrbit;       //轨迹类型

	MAC_ELEC_PARA* m_pElecPara;	
	MAC_OPERATE_TYPE m_enOpType;
public:
	BOOL SetEdmOpFile(QString sPath,QString sFile);
	void SetEdmOpElec(QString str,MAC_ELEC_PARA elec,BOOL bStart,BOOL bCycleStart,int iPageIndex);
    BOOL IsStrElecCmd(QString str);
    BOOL IsPauseCmd(QString str);
    BOOL IsOverCmd(QString str);
    BOOL IsMillStart(QString str);
    BOOL IsMillOver(QString str);
	void SetEdmElecIndex(int iCmdIndex);
	void PlusDigit2Cmd();
    BOOL IsMillFile(QString str);
	void SaveElec2Db();
    QString GetLastElecName(int iCmdIndex);
	void SetOpType(MAC_OPERATE_TYPE enOpType);
private:
	void CalcStartPt(DIGIT_CMD* pCmd,int iSum[]);
};


