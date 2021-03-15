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
#include "setting.h"
#include "unionzero.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~ MainWindow();

public:
    EDM* edm;
    MAC_SYSTEM_SET mSysSet;
    EDM_COOR_TYPE m_enCoorType;
private:
    unsigned char EDMMacInit();
    void MacUserOperate();
    void createMenus();
    void createActions();
    void Char2QStringInBinary(unsigned char btVal,QString &str);
private:
    MAC_SYSTEM_SET m_stSysSet;
    QString m_strOpName;
    int  m_iOpenTime;
    int m_iOpenTimeOp;
    MAC_INTERFACE_IN mIn;
    MAC_INTERFACE_OUT mOut;
    QFuture<void> macUserHandle;
    QThread* tThread;
    QMutex mutex;
    Process* process;
    Program* program;
    Setting* setting;
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
    CoordWidget* coordWidget = nullptr;
    //right
    AlarmSignal* alarmSignal = nullptr;
    QLabel* purgeValue;
    QLabel* shakeValue;
    QLabel* protectValue;
    QLabel* highFreqValue;
    QLabel* rotateValue;
    QLabel* fixElecValue;
    QPushButton* axisZero;
    QPushButton* axisSet;
    QPushButton* findCenter;
    QGridLayout* rightLayout;
    //debug
    QTableWidget* tv1;
    bool bPrint;
    //bottom
    QLabel* commandLabel;
    QLineEdit* commandLine;
    QComboBox* speedValue;
    QHBoxLayout* bottomLayout;

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
};

#endif // MAINWINDOW_H
