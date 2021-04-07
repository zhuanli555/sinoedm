#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QMutex>
#include <QMutexLocker>
#include <QGridLayout>
#include "EDM/EDM.h"
#include "EDM/cmdhandle.h"
#include "coordwidget.h"
#include "alarmsignal.h"
#include "program.h"
#include "settingdialog.h"
#include "unionzero.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~ MainWindow();

public:

private:
    unsigned char EDMMacInit();
    void MacUserOperate();
    void MacProcessOperate();
    void HandleOpMsg();
    void HandleEdmOpStatus();
    void showFileText();
    void createMenus();
    void createActions();
    QWidget* createCommandTab();
    QWidget* createProcessTab();
    void Char2QStringInBinary(unsigned char btVal,QString &str);
    void systemSetChangeForCoord();
    void OpFileCopyAndSend();
    void fillTableWidget(MAC_ELEC_PARA* pPara);
    void ReadParaFromTable(MAC_ELEC_PARA* pPara);
    int PercentStr2int(QString str);
    void LawOfPara(MAC_ELEC_PARA* pPara);
    void LawInt(int& t,int low,int high);

private:
    EDM* edm;
    MAC_SYSTEM_SET mSysSet;
    EDM_COOR_TYPE m_enCoorType;
    EDM_OP_List* edmOpList;
    EDM_OP* edmOp;
    MAC_SYSTEM_SET m_stSysSet;
    MAP_ELEC_MAN m_mpElecMan;       //电参数管理;
    int  m_iOpenTime;
    int m_iOpenTimeOp;
    MAC_INTERFACE_IN mIn;
    MAC_INTERFACE_OUT mOut;
    QFuture<void> macUserHandle;
    QFuture<void> macProcessHandle;
    QMutex mutex;
    QString m_strElecName;
    QLabel *fileLabel;
    Program* program;
    SettingDialog* setting;
    UnionZero* unionZero;
    QStatusBar* statBar;
    QTextEdit* commandText;
    QAction* stopAction;
    QAction* processAction;
    QAction* unionZeroAction;
    QAction* axisSetAction;
    QAction* programAction;
    QAction* simulateAction;
    QAction* settingAction;
    QAction* workZeroAction;
    QAction* exitAction;
    //left
    CoordWidget* coordWidget;
    //right
    AlarmSignal* alarmSignal;
    QLabel* purgeValue;
    QLabel* shakeValue;
    QLabel* protectValue;
    QLabel* highFreqValue;
    QLabel* rotateValue;
    QLabel* fixElecValue;
    QPushButton* axisZero;
    QPushButton* axisSet;
    QPlainTextEdit* fileText;
    QGridLayout* rightLayout;
    //debug
    QTableWidget* tv1;
    bool bPrint;
    QTabWidget* tab;
    //bottom
    QLabel* commandLabel;
    QLineEdit* commandLine;
    QComboBox* speedValue;
    QTableWidget* elecPageTable;
    QTableWidget* elecOralTable;
    bool m_quit = false;
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void timeUpdate();
signals:
    void coordWidgetChanged();
    void edmOPSig(MAC_OPERATE_TYPE);
    void edmCloseSig();
    void edmMoveParaSendSig(DIGIT_CMD*);
    void edmOpFileSig(QString,QString);
    void edmOpElecSig(QString,MAC_ELEC_PARA);
    void edmWriteElecSig(Elec_Page*,QString);
    void edmPauseSig();
    void edmShakeSig();
    void edmPurgeSig();
    void edmStopSig();
    void systemSetChangeSig();

protected slots:

    void renderToProcess();
    void renderToSimulate();
    void renderToProgram();
    void renderToSetting();
    void renderToUnionZero();
    void renderToWorkZero();
    void renderToAxisSet();
    void edmStop();
    void edmSendComand();
    void elecTableChanged();
    void printInterface();
};

#endif // MAINWINDOW_H
