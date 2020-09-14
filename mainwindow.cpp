#include "mainwindow.h"
#include "EDM/initdb.h"
#include <QDialog>
#include <QtGui/QKeyEvent>
#include <QTimer>
#include <QDateTime>
#include <QGridLayout>
#include <QDebug>
#include <QTextCodec>
#include <QApplication>
#include <QDesktopWidget>
#include "process.h"
#include "program.h"
#include "setting.h"
#include "unionzero.h"
#include <QtConcurrent>

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QWidget* widget = new QWidget();
    this->setCentralWidget(widget);
    setWindowTitle(QString::fromLocal8Bit("数控机床v1.0"));
    setGeometry(0,0,QApplication::desktop()->availableGeometry().width(),QApplication::desktop()->availableGeometry().height());
    //获取edm实例
    //init edm
    EDMMacInit();
    //状态栏
    statBar = statusBar();
    //left
    coordWidget = new CoordWidget();
    //right
    alarmSignal = new AlarmSignal();
    axisSet = new QPushButton(QString::fromLocal8Bit("轴置数Ctrl/?"));
    axisZero = new QPushButton(QString::fromLocal8Bit("轴清零Alt/?"));
    findCenter = new QPushButton(QString::fromLocal8Bit("找中心(F)"));
    connect(findCenter,&QPushButton::clicked,this,&MainWindow::edmFindCenter);

    rightLayout = new QGridLayout();
    rightLayout->setSpacing(20);
    rightLayout->addWidget(axisSet,0,0);
    rightLayout->addWidget(axisZero,1,0);
    rightLayout->addWidget(findCenter,2,0);
    rightLayout->addWidget(alarmSignal,0,1,6,1);
    findCenter->setMaximumWidth(85);
    rightLayout->setSizeConstraint(QLayout::SetFixedSize);
    //bottom
    commandLabel = new QLabel(QString::fromLocal8Bit("命令"));
    commandLine = new QLineEdit();
    connect(commandLine,&QLineEdit::returnPressed,this,&MainWindow::edmSendComand);
    QLabel *speedLabel = new QLabel(QString::fromLocal8Bit("定位速度:"));
    speedValue = new QLabel(QString::fromLocal8Bit("mid"));
    bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(commandLabel);
    bottomLayout->addWidget(commandLine);
    bottomLayout->addWidget(speedLabel);
    bottomLayout->addWidget(speedValue);
    //main
    QGridLayout *mainLayout =new QGridLayout();

    mainLayout->setMargin(15);					//设定对话框的边距为15
    mainLayout->setSpacing(10);
    mainLayout->addWidget(coordWidget,0,0);
    mainLayout->addLayout(rightLayout,0,1);
    mainLayout->addLayout(bottomLayout,1,0,1,2);
    mainLayout->setColumnStretch(0,1);//设置1:1
    mainLayout->setColumnStretch(1,1);
    widget->setLayout(mainLayout);
    createActions();
    createMenus();
    //设置多线程信号
    tThread = new QThread();
    edm->moveToThread(tThread);
    connect(tThread,&QThread::finished,tThread,&QObject::deleteLater);
    connect(tThread,&QThread::finished,edm,&QObject::deleteLater);
    tThread->start();
    //设置定时器
    QTimer *t = new QTimer(this);
    connect(t,&QTimer::timeout,this,&MainWindow::timeUpdate);
    t->start(1000);

}

MainWindow::~MainWindow()
{
    if(tThread)
    {
        tThread->quit();
    }
    tThread->wait();
}

BOOL MainWindow::EDMMacInit()
{
    BOOL bInit;
    edm =  EDM::GetEdmInstance();
    bInit = edm->EdmInit();
    edm->GetMacPara(&m_stSysSet);
    m_iOpenTime = m_stSysSet.stSetNoneLabel.iTime;
    m_iOpenTimeOp = m_stSysSet.stSetNoneLabel.iTimeOp;
    return bInit;
}

void MainWindow::timeUpdate()
{
    QDateTime t = QDateTime::currentDateTime();
    int tt = t.toTime_t();
    float ll = tt%1000000;
    QString s = QString::number(ll/1000,'f',3);
    statBar->showMessage(s);
    coordWidget->HandleEdmCycleData();//coordwidget 循环
    //发送command循环
    //其他循环
    //测试alarm
//    QString strr=QString::fromLocal8Bit("<font color=\"#FF0000\">警告：%1发送频率过快!</font> ").arg(0);
//    alarmText->append(strr);
}

void MainWindow::createActions()
{
    processAction = new QAction(QString::fromLocal8Bit("加工(F1)"),this);
    processAction->setShortcut(tr("F1"));
    processAction->setStatusTip("加工文件");
    connect(processAction,&QAction::triggered,this,&MainWindow::renderToProcess);

    unionZeroAction = new QAction(QString::fromLocal8Bit("回零(F2)"),this);
    unionZeroAction->setShortcut(tr("F2"));
    unionZeroAction->setStatusTip("回零");
    connect(unionZeroAction,&QAction::triggered,this,&MainWindow::renderToUnionZero);

    programAction = new QAction(QString::fromLocal8Bit("编程(F3)"),this);
    programAction->setShortcut(tr("F3"));
    programAction->setStatusTip("编程文件");
    connect(programAction,&QAction::triggered,this,&MainWindow::renderToProgram);

    settingAction = new QAction(QString::fromLocal8Bit("设置(F4)"),this);
    settingAction->setShortcut(tr("F4"));
    settingAction->setStatusTip("设置");
    connect(settingAction,&QAction::triggered,this,&MainWindow::renderToSetting);

}

void MainWindow::createMenus()
{
    QMenuBar* myMenu = menuBar();
    myMenu->setStyleSheet("QMenuBar::item{\
      background-color:rgb(89,87,87);margin:2px 2px;color:yellow;}\
       QMenuBar::item:selected{background-color:rgb(235,110,36);}\
        QMenuBar::item:pressed{background-color:rgb(235,110,6);border:1px solid rgb(60,60,60);}");
    myMenu->addAction(processAction);
    myMenu->addAction(unionZeroAction);
    myMenu->addAction(programAction);
    myMenu->addAction(settingAction);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if((e->modifiers() == Qt::ControlModifier) && (e->key() == Qt::Key_Slash)){

        axisDialog = new AxisSetDialog();
        axisDialog->exec();
    }else{
        if((e->modifiers() == Qt::ControlModifier) && (e->key() == Qt::Key_X ||
                                                       e->key() == Qt::Key_Y ||
                                                       e->key() == Qt::Key_W ||
                                                       e->key() == Qt::Key_Z ||
                                                       e->key() == Qt::Key_A ||
                                                       e->key() == Qt::Key_B)){

            axisDialog = new AxisSetDialog(e->key(),this);
            int res = axisDialog->exec();
            if(res == QDialog::Accepted)
            {
                axisDialog->axisSet();
                delete axisDialog;
            }
        }
    }
    //轴清零
    if((e->modifiers() == Qt::AltModifier) && (e->key() == Qt::Key_Slash)){

        axisDialog = new AxisSetDialog();
        axisDialog->exec();
    }else{
        if((e->modifiers() == Qt::AltModifier) && (e->key() == Qt::Key_X ||
                                                       e->key() == Qt::Key_Y ||
                                                       e->key() == Qt::Key_W ||
                                                       e->key() == Qt::Key_Z ||
                                                       e->key() == Qt::Key_A ||
                                                       e->key() == Qt::Key_B)){

            //TODO这部分代码不应该放在这
            QChar m_str(e->key());
            CmdHandle *pCmdHandle;
            static DIGIT_CMD stDigitCmd;
            DIGIT_CMD cmdDefault;
            QString cmd = "G92 G00";
            cmd += m_str;
            cmd += "0";
            cmdDefault.enCoor = edm->m_stEdmShowData.enCoorType;
            pCmdHandle = new CmdHandle(FALSE,cmd,&stDigitCmd,&cmdDefault);
            delete pCmdHandle;
            edm->EdmSendMovePara(&stDigitCmd);
            //emit signalSendMovePara(&stDigitCmd);
        }
    }

    switch (e->key()) {
    case Qt::Key_Escape:
        close();
        break;
    case Qt::Key_F4:
        edmPurge();
    case Qt::Key_F5:
        close();
    case Qt::Key_F6:
        edmProtect();
    case Qt::Key_F7:
        edmShake();
    case Qt::Key_F8:
        close();
    case Qt::Key_F9:
        close();
    case Qt::Key_F10:
        close();
    default:
        break;
    }


}

void MainWindow::renderToProcess()
{
    Process* process = new Process();
    process->show();
}

void MainWindow::renderToProgram()
{
    Program* program = new Program();
    program->show();
}

void MainWindow::renderToSetting()
{
    Setting* setting = new Setting();
    setting->show();
}

void MainWindow::renderToUnionZero()
{
    UnionZero* unionZero = new UnionZero();
    int res = unionZero->exec();
    if(res != QDialog::Accepted)return;
    unionZero->show();
}

void MainWindow::edmProtect()
{
    //多线程
    edm->EdmSetProtect(edm->m_stEdmShowData.stStatus.bNoProtect);
    if(edm->m_stEdmShowData.stStatus.bNoProtect)
    {
        protectValue->setStyleSheet("background-color:green;");
    }else{
        protectValue->setStyleSheet("background-color:red;");
    }
}

void MainWindow::edmPurge()
{
    edm->EdmLowPump(!edm->m_stEdmShowData.stStatus.bPumpLow);//低压 冲液？
    if(edm->m_stEdmShowData.stStatus.bPumpLow)
    {
        purgeValue->setStyleSheet("background-color:red;");
    }
    else{
        purgeValue->setStyleSheet("background-color:green;");
    }
}

void MainWindow::edmShake()
{
    edm->EdmSetShake(edm->m_stEdmShowData.stStatus.bShake);
    if(edm->m_stEdmShowData.stStatus.bShake)
    {
        shakeValue->setStyleSheet("background-color:green;");
    }else{
        shakeValue->setStyleSheet("background-color:red;");
    }
}

void MainWindow::edmHighFreq()
{
    edm->EdmPower(!edm->m_stEdmShowData.stStatus.bPower);
    if(edm->m_stEdmShowData.stStatus.bPower)
    {
        highFreqValue->setStyleSheet("background-color:red;");
    }
    else{
        highFreqValue->setStyleSheet("background-color:green;");
    }
}

void MainWindow::edmFindCenter()
{
    edm->EdmPower(!edm->m_stEdmShowData.stStatus.bPower);
    if(edm->m_stEdmShowData.stStatus.bPower)
    {
        highFreqValue->setStyleSheet("background-color:red;");
    }
    else{
        highFreqValue->setStyleSheet("background-color:green;");
    }
}

void MainWindow::edmSendComand()
{
    static DIGIT_CMD stDigitCmd;
    DIGIT_CMD cmdDefault;
    CmdHandle* pCmdHandle;
    int speed;
    if(speedValue->text() == "mid")
    {
        speed = 10000;
    }else if(speedValue->text() == "low")
    {
        speed = 5000;
    }else{
        speed = 20000;
    }
    memset(&cmdDefault,0,sizeof(DIGIT_CMD));
    cmdDefault.enAim = AIM_G91;
    cmdDefault.enOrbit = ORBIT_G00;
    cmdDefault.enCoor = edm->m_stEdmShowData.enCoorType;
    cmdDefault.iFreq = CmdHandle::GetSpeedFreq(speed);

    pCmdHandle = new CmdHandle(FALSE,commandLine->text(),&stDigitCmd,&cmdDefault);
    delete pCmdHandle;
    stDigitCmd.stOp.bShortDis = TRUE;
    stDigitCmd.bNoCheck = TRUE;
    edm->EdmSendMovePara(&stDigitCmd);
}
