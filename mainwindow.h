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
#include <QLineEdit>
#include <QTextEdit>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QGridLayout>
#include "EDM/EDM.h"
#include "EDM/cmdhandle.h"
#include "coordwidget.h"
#include "alarmsignal.h"
#include "process.h"
#include "program.h"
#include "settingdialog.h"
#include "unionzero.h"
struct testName
{
    int a;
    int b;
    int c;
};
union FILE_READBUFFER{
    struct testName m_param;
    char header_pt[sizeof(m_param)];
};
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
    void setAxisValue(int label,QString str);
    void systemSetChangeForCoord();
private:
    EDM* edm;
    MAC_SYSTEM_SET mSysSet;
    EDM_COOR_TYPE m_enCoorType;
    EDM_OP_List* edmOpList;
    EDM_OP* edmOp;
    MAC_SYSTEM_SET m_stSysSet;
    QString m_strOpName;
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
    //bottom
    QLabel* commandLabel;
    QLineEdit* commandLine;
    QComboBox* speedValue;

    bool m_quit = false;
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void timeUpdate();
protected slots:

    void renderToProcess();
    void renderToProgram();
    void renderToSetting();
    void renderToUnionZero();
    void renderToWorkZero();
    void renderToAxisSet();
    void edmStop();
    void edmSendComand();
    void printInterface();
    void refresh();
};

#endif // MAINWINDOW_H
