#pragma once

#include <QObject>
#include "EDM_OP_HOLE.h"

class EDM_OP_List:public QObject
{
    Q_OBJECT
private:
    explicit EDM_OP_List(QObject *parent = nullptr);
	virtual  ~EDM_OP_List();
public:
	static EDM_OP_List* GetEdmOpListPtr();
    static void DeleteEdmOpList();
	void SetEdmOpType(MAC_OPERATE_TYPE enType);
	void SetStart(bool bStart);
	void CarryOn();
	void StageRestart();
    void EdmOpListOver();
	void DeleteEdmOp();
public:
	unsigned char m_bChange;
	EDM_OP* m_pEdmOp;
	unsigned char m_bOver;
private:
	static EDM_OP_List* m_pEdmOpList;
	MAC_OPERATE_TYPE m_enOpType;
	
	QString m_sPath;
    QString m_sFile;
public slots:
    void CarryOnBefore(MAC_OPERATE_TYPE enType);
	unsigned char SetEdmOpFile(QString sPath,QString sFile);
};


