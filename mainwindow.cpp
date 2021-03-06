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
#include <QtConcurrent>

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QWidget* widget = new QWidget();
    this->setCentralWidget(widget);
    setWindowTitle(QString::fromLocal8Bit("数控机床v1.0"));
    setGeometry(0,0,QApplication::desktop()->width(),QApplication::desktop()->height());
    //init db
    initDb();
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
    //添加一个lineedit 记录发送的命令
    commandText = new QTextEdit();
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
    speedValue = new QComboBox();
    speedValue->clear();
    QStringList speedlist;
    speedlist<<"1000"<<"2000"<<"5000"<<"10000"<<"20000";
    speedValue->addItems(speedlist);
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
    mainLayout->addWidget(commandText,1,0,1,2);
    mainLayout->addLayout(bottomLayout,2,0,1,2);
    mainLayout->setColumnStretch(0,1);//设置1:1
    mainLayout->setColumnStretch(1,1);
    widget->setLayout(mainLayout);
    createActions();
    createMenus();
    //设置多线程信号
    QtConcurrent::run(this,&MainWindow::MacUserOperate);
    //设置定时器
    QTimer *t = new QTimer(this);
    connect(t,&QTimer::timeout,this,&MainWindow::timeUpdate);
    t->start(1000);

}

MainWindow::~MainWindow()
{
    if(edm)
    {
        edm->EdmStop();
        edm->EdmClose();
        EDM::DelEdm();
    }
    coordWidget->close();
    alarmSignal->close();
    axisDialog->close();
    process->close();
    program->close();
    setting->close();
    unionZero->close();
}

void MainWindow::MacUserOperate()
{
    while(true)
    {
        if(edm)
        {
            mutex.lock();
            coordWidget->HandleEdmCycleData();//机床命令周期性处理
            alarmSignal->EdmStatusSignChange();//机床信号周期性处理
            mutex.unlock();
        }
        QThread::msleep(20);
    }
}

unsigned char MainWindow::EDMMacInit()
{
    unsigned char bInit;
    edm =  EDM::GetEdmInstance();
    bInit = edm->EdmInit();
    edm->GetMacPara(&m_stSysSet);
    m_iOpenTime = m_stSysSet.stSetNoneLabel.iTime;
    m_iOpenTimeOp = m_stSysSet.stSetNoneLabel.iTimeOp;
    return bInit;
}

void MainWindow::timeUpdate()
{
//    QDateTime t = QDateTime::currentDateTime();
//    int tt = t.toTime_t();
//    float ll = tt%1000000;
//    QString s = QString::number(ll/1000,'f',3);
//    statBar->showMessage(s);
    //发送command循环
    //其他循环
    //测试alarm
//    QString strr=QString::fromLocal8Bit("<font color=\"#FF0000\">警告：%1发送频率过快!</font> ").arg(0);
//    alarmText->append(strr);
}

void MainWindow::createActions()
{
    stopAction = new QAction(QString::fromLocal8Bit("总停(ESC)"),this);
    stopAction->setShortcut(tr("ESC"));
    stopAction->setStatusTip(tr("总停"));
    connect(stopAction,&QAction::triggered,this,&MainWindow::edmStop);

    processAction = new QAction(QString::fromLocal8Bit("加工(F1)"),this);
    processAction->setShortcut(tr("F1"));
    processAction->setStatusTip(tr("加工文件"));
    connect(processAction,&QAction::triggered,this,&MainWindow::renderToProcess);

    unionZeroAction = new QAction(QString::fromLocal8Bit("回零(F2)"),this);
    unionZeroAction->setShortcut(tr("F2"));
    unionZeroAction->setStatusTip(tr("回零"));
    connect(unionZeroAction,&QAction::triggered,this,&MainWindow::renderToUnionZero);

    programAction = new QAction(QString::fromLocal8Bit("编程(F3)"),this);
    programAction->setShortcut(tr("F3"));
    programAction->setStatusTip(tr("编程文件"));
    connect(programAction,&QAction::triggered,this,&MainWindow::renderToProgram);

    settingAction = new QAction(QString::fromLocal8Bit("设置(F10)"),this);
    settingAction->setShortcut(tr("F10"));
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
    myMenu->addAction(stopAction);
    myMenu->addAction(processAction);
    myMenu->addAction(unionZeroAction);
    myMenu->addAction(programAction);
    myMenu->addAction(settingAction);
    //add stop menu,close menu

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
            QString cmd = "G90 G00";
            cmd += m_str;
            cmd += "0";
            cmdDefault.enCoor = edm->m_stEdmShowData.enCoorType;
            pCmdHandle = new CmdHandle(FALSE,cmd,&stDigitCmd,&cmdDefault);
            delete pCmdHandle;
            edm->EdmSendMovePara(&stDigitCmd);
        }
    }

    switch (e->key()) {
    case Qt::Key_Escape:
        edmStop();break;
    case Qt::Key_F4:
        alarmSignal->edmPurge();break;
    case Qt::Key_F5:
        alarmSignal->edmLowerPump();break;
    case Qt::Key_F6:
        alarmSignal->edmShake();break;
    case Qt::Key_F7:
        alarmSignal->edmProtect();break;
    case Qt::Key_F8:
        alarmSignal->edmPause();break;
    case Qt::Key_F12:
        close();break;
    default:
        break;
    }


}

void MainWindow::renderToProcess()
{
    process = new Process();
    process->show();
}

void MainWindow::renderToProgram()
{
    program = new Program();
    program->show();
}

void MainWindow::renderToSetting()
{
    setting = new Setting();
    setting->show();
}

void MainWindow::renderToUnionZero()
{
    unionZero = new UnionZero();
    int res = unionZero->exec();
    if(res != QDialog::Accepted)return;
    unionZero->show();
}

void MainWindow::edmStop()
{
    alarmSignal->edmStop();
}

void MainWindow::edmSendComand()
{
    static DIGIT_CMD stDigitCmd;
    DIGIT_CMD cmdDefault;
    CmdHandle* pCmdHandle;
    QString cmdstr;
    int speed = speedValue->currentText().toInt();
    cmdstr = commandLine->text();
    commandText->append(cmdstr);
    memset(&cmdDefault,0,sizeof(DIGIT_CMD));
    cmdDefault.enAim = AIM_G91;
    cmdDefault.enOrbit = ORBIT_G00;
    cmdDefault.enCoor = edm->m_stEdmShowData.enCoorType;
    cmdDefault.iFreq = speed;
    pCmdHandle = new CmdHandle(FALSE,cmdstr,&stDigitCmd,&cmdDefault);
    delete pCmdHandle;
    stDigitCmd.stOp.bShortDis = TRUE;
    stDigitCmd.bNoCheck = TRUE;
    edm->EdmSendMovePara(&stDigitCmd);
}
