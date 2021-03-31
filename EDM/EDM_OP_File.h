#pragma once

#include "EDM.h"
#include "cmdhandle.h"

typedef enum
{
	OP_FILE_NO_ERR = 0,             //����
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
    vector<QString> m_vCmd;         //�ӹ�����
    vector<QString> m_vCmdStd;      //��׼�ĳ���
    vector<QString> m_vCmdLoc;
	MAP_ELEC_MAN m_mpElecMan;       //���������	
    QString m_strElec;              //��ǰ�絥������
	OP_FILE_ERROR m_enOpFileErr;    //�ӹ��ļ��д���

	int m_iCmdNum;                  //�ܵ���Ŀ
	QString m_sFile;	            //�ļ�����
	QString m_sPath;              //·��

	EDM_COOR_TYPE  m_enCoor;
protected:
	EDM* m_pEdm;	

	EDM_AIM_TYPE m_enAim;           //�˶�Ŀ������
	EDM_ORBIT_TYPE m_enOrbit;       //�켣����

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


