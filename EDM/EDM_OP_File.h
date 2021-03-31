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
	unsigned char SetEdmOpFile(QString sPath,QString sFile);
	void SetEdmOpElec(QString str,MAC_ELEC_PARA elec,unsigned char bStart,unsigned char bCycleStart,int iPageIndex);
    unsigned char IsPauseCmd(QString str);
    unsigned char IsOverCmd(QString str);
    unsigned char IsMillStart(QString str);
    unsigned char IsMillOver(QString str);
	void SetEdmElecIndex(int iCmdIndex);
    void PlusDigit2Cmd();
	void SetOpType(MAC_OPERATE_TYPE enOpType);
private:
	void CalcStartPt(DIGIT_CMD* pCmd,int iSum[]);
};


