#ifndef PROCESS_H
#define PROCESS_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>
#include <QModelIndex>
#include <QStatusBar>
#include <QAction>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QTableView>
#include <QGridLayout>
#include "EDM/EDM.h"
#include "program.h"
#include "coordwidget.h"
#include "alarmsignal.h"
#include "EDM/cmdhandle.h"
#include "EDM/EDM_OP_List.h"
#include "elecparatable.h"

class Process : public QMainWindow
{
    Q_OBJECT
public:
    Process(QWidget *parent = nullptr);
    ~ Process();
public:
    QString m_strElecName;
    BOOL bPause;
private:
    BOOL EDMProcessInit();
    void showFileText();
    void createMenus();
    void createActions();
    void MacProcessOperate();
    void HandleOpMsg();
    void HandleEdmOpStatus();
private:
    EDM* edm;
    EDM_OP_List* edmOpList;
    EDM_OP* edmOp;
    MSG_TYPE gMsg;
    MAC_OPERATE_TYPE enType;
    QString strSysPath;
    QString strOpName;
    QThread* tThread;
    QMutex mutex;
    Program* program;
    QStatusBar* statBar;
    QAction* processAction;
    QAction* imitateAction;
    QAction* pauseAction;
    //left
    CoordWidget* coordWidget;
    //right
    QString gFilename;
    QLabel *fileLabel;
    QPlainTextEdit* fileText;
    AlarmSignal* alarmSignal;
    QGridLayout* rightLayout;
    //bottom
    QHBoxLayout* bottomLayout;
    ElecParaTable *elecParaTable;
    QLineEdit *jxProcessVal;
    QLineEdit *ProcessNumVal;
    QLineEdit *singleProcessVal;
    QLineEdit *safePlaceVal;
    QLineEdit *backPlaceVal;
    QLineEdit *smallTimeVal;
    QLineEdit *bigTimeVal;
    QLineEdit *shakeHzVal;
    QLineEdit *bottomTimeVal;
    QLineEdit *daoZhuiVal;
    QLineEdit *elecLenVal;
    QLabel *findCenterVal ;
    QLabel *totalTimeVal ;
    QLabel *processTimeVal ;
    QLabel *st1Val ;
    QLabel *st2Val ;
    QLabel *st3Val ;
    QLabel *st4Val ;
    QLabel *st5Val ;
    QLabel *st6Val ;
    QLabel *st7Val ;
    QLabel *st8Val;
    QLabel *mainAxisUp;
    QLabel *confirmHole;
    QLabel *elecZero;
    QLabel *elecFix;
    QLabel *elecProcess;
    QSqlRelationalTableModel *elecPageModel;
    QSqlTableModel *elecOralModel;
protected:
    void keyPressEvent(QKeyEvent *) override;

protected slots:

    void timeUpdate();
    void pause();
    void programProcess();
    void imitateProcess();
};


#endif // PROCESS_H
