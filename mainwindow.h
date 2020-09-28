#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QGridLayout>
#include "EDM/EDM.h"
#include "coordwidget.h"
#include "alarmsignal.h"
#include "axissetdialog.h"
#include "EDM/cmdhandle.h"

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
    BOOL EDMMacInit();
    void MacUserOperate();
    void createMenus();
    void createActions();
private:
    MAC_SYSTEM_SET m_stSysSet;
    QString m_strOpName;
    int  m_iOpenTime;
    int m_iOpenTimeOp;

    QThread* tThread;
    QMutex mutex;
    QMenu* processMenu;
    QMenu* programMenu;
    QMenu* settingMenu;
    QStatusBar* statBar;
    QTextEdit* alarmText;
    QAction* processAction;
    QAction* unionZeroAction;
    QAction* programAction;
    QAction* settingAction;
    QAction* purgeAction;
    QAction* shakeAction;
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
    QPushButton* findCenter;
    AxisSetDialog* axisDialog;
    QGridLayout* rightLayout;
    //bottom
    QLabel* commandLabel;
    QLineEdit* commandLine;
    QLabel* speedValue;
    QHBoxLayout* bottomLayout;
protected:
    void keyPressEvent(QKeyEvent *e) override;

protected slots:
    void renderToProcess();
    void renderToProgram();
    void renderToSetting();
    void renderToUnionZero();
    void edmProtect();
    void edmPurge();
    void edmShake();
    void edmHighFreq();
    void edmSendComand();
    void edmFindCenter();
    void timeUpdate();
};

#endif // MAINWINDOW_H
