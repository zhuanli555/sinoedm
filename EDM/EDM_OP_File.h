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

class EDM_OP_File
{
public:
	EDM_OP_File();
	virtual  ~EDM_OP_File();
public:
    vector<QString> m_vCmd;         //����ļӹ�����
    vector<QString> m_vCmdStd;      //��׼�ĳ���
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
    unsigned char IsPauseCmd(QString str);
    unsigned char IsOverCmd(QString str);
    unsigned char IsMillStart(QString str);
    unsigned char IsMillOver(QString str);
    void PlusDigit2Cmd();
	void SetOpType(MAC_OPERATE_TYPE enOpType);
private:
    unsigned char ReadCmdFromFile(QString strPath,QString strFile);
};


