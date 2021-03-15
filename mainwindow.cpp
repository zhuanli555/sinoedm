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
    coordWidget = CoordWidget::getInstance();
    //right
    alarmSignal = AlarmSignal::getInstance();
    findCenter = new QPushButton(QString::fromLocal8Bit("找中心(F)"));
    //添加一个lineedit 记录发送的命令
    commandText = new QTextEdit();
    rightLayout = new QGridLayout();
    rightLayout->setSpacing(20);
    rightLayout->addWidget(findCenter,1,0);
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

    //debug
    QHBoxLayout* midLayout = new QHBoxLayout();
    tv1 = new QTableWidget;
    tv1->setMaximumWidth(0);
    midLayout->addWidget(commandText);
    midLayout->addWidget(tv1);
    mainLayout->addLayout(midLayout,1,0,1,2);

    mainLayout->addLayout(bottomLayout,2,0,1,2);
    mainLayout->setColumnStretch(0,1);//设置1:1
    mainLayout->setColumnStretch(1,1);
    widget->setLayout(mainLayout);
    createActions();
    createMenus();
    //设置多线程信号
    macUserHandle = QtConcurrent::run(this,&MainWindow::MacUserOperate);

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
    mutex.lock();
    m_quit = true;
    mutex.unlock();
    macUserHandle.waitForFinished();
}

void MainWindow::MacUserOperate()
{
    while(!m_quit)
    {
        if(edm)
        {
            mutex.lock();
            coordWidget->HandleEdmCycleData();//机床命令周期性处理
            alarmSignal->EdmStatusSignChange();//机床信号周期性处理

            mutex.unlock();
        }
        QThread::msleep(35);
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
    memset(&mIn,0,sizeof(MAC_INTERFACE_IN));
    memset(&mOut,0,sizeof(MAC_INTERFACE_OUT));
    bPrint = false;
    return bInit;
}

void MainWindow::timeUpdate()
{
    if(bPrint)//打印接口
    {
        printInterface();
    }
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

    unionZeroAction = new QAction(QString::fromLocal8Bit("回机械零(F2)"),this);
    unionZeroAction->setShortcut(tr("F2"));
    unionZeroAction->setStatusTip(tr("回机械零"));
    connect(unionZeroAction,&QAction::triggered,this,&MainWindow::renderToUnionZero);

    programAction = new QAction(QString::fromLocal8Bit("编程(F3)"),this);
    programAction->setShortcut(tr("F3"));
    programAction->setStatusTip(tr("编程文件"));
    connect(programAction,&QAction::triggered,this,&MainWindow::renderToProgram);

    workZeroAction = new QAction(QString::fromLocal8Bit("回工作零(Ctrl-R)"),this);
    workZeroAction->setShortcut(tr("Ctrl+R"));
    workZeroAction->setStatusTip(tr("回工作零"));
    connect(workZeroAction,&QAction::triggered,this,&MainWindow::renderToWorkZero);

    axisSetAction = new QAction(QString::fromLocal8Bit("轴置数(Ctrl-/)"),this);
    axisSetAction->setShortcut(tr("Ctrl+/"));
    axisSetAction->setStatusTip(tr("轴置数"));
    connect(axisSetAction,&QAction::triggered,this,&MainWindow::renderToAxisSet);

    settingAction = new QAction(QString::fromLocal8Bit("设置(F10)"),this);
    settingAction->setShortcut(tr("F10"));
    settingAction->setStatusTip("设置");
    connect(settingAction,&QAction::triggered,this,&MainWindow::renderToSetting);

    exitAction = new QAction(QString::fromLocal8Bit("退出(Ctrl-Q)"),this);
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip("退出");
    connect(exitAction,&QAction::triggered,this,&MainWindow::close);

}

void MainWindow::createMenus()
{
    QMenuBar* myMenu = menuBar();
    myMenu->setStyleSheet("QMenuBar::item{\
      background-color:rgb(89,87,87);margin:2px 2px;color:yellow;border-bottom:1px solid rgb(237, 238, 241);}\
       QMenuBar::item:selected{background-color:rgb(235,110,36);}\
        QMenuBar::item:pressed{background-color:rgb(235,110,6);border:1px solid rgb(60,60,60);}");

    myMenu->addAction(stopAction);
    myMenu->addAction(processAction);
    myMenu->addAction(unionZeroAction);
    myMenu->addAction(programAction);
    myMenu->addAction(workZeroAction);
    myMenu->addAction(axisSetAction);
    myMenu->addAction(settingAction);
    myMenu->addAction(exitAction);

}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
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
    {
        bPrint = !bPrint;
        if(!bPrint){
            if(tv1->width()>0)tv1->setMaximumWidth(0);
        }
        break;
    }
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
    unionZero = new UnionZero(0);
    int res = unionZero->exec();
    if(res != QDialog::Accepted)return;
    delete unionZero;
}

void MainWindow::renderToWorkZero()
{
    unionZero = new UnionZero(1);
    int res = unionZero->exec();
    if(res != QDialog::Accepted)return;
    delete unionZero;
}

void MainWindow::renderToAxisSet()
{
    unionZero = new UnionZero(2);
    int res = unionZero->exec();
    if(res != QDialog::Accepted)return;
    delete unionZero;
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

void MainWindow::Char2QStringInBinary(unsigned char btVal,QString &str)
{
    str = "";
    int i=0;
    static unsigned char btCom[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
    for (;i<8;i++)
    {
        if (btVal & btCom[i])
            str +="1";
        else
            str +="0";
    }
}

void MainWindow::printInterface()
{
    QString str;
    tv1->setMaximumWidth(320);tv1->setColumnCount(2);tv1->setRowCount(10);
    tv1->setColumnWidth(0,160);tv1->setColumnWidth(1,160);
    tv1->setHorizontalHeaderItem(0,new QTableWidgetItem(QString("PORT OUT")));
    tv1->setHorizontalHeaderItem(1,new QTableWidgetItem(QString("PORT IN")));
    //setOut
    if(edm->m_stEdmShowData.stEdmInterfaceOut.btO140 != mOut.btO140)
    {
        mOut.btO140 = edm->m_stEdmShowData.stEdmInterfaceOut.btO140;
        Char2QStringInBinary(mOut.btO140,str);
        tv1->setItem(0,0,new QTableWidgetItem(QString("O140    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceOut.btO144 != mOut.btO144)
    {
        mOut.btO144 = edm->m_stEdmShowData.stEdmInterfaceOut.btO144;
        Char2QStringInBinary(mOut.btO144,str);
        tv1->setItem(1,0,new QTableWidgetItem(QString("O144    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceOut.btO184 != mOut.btO184)
    {
        mOut.btO184 = edm->m_stEdmShowData.stEdmInterfaceOut.btO184;
        Char2QStringInBinary(mOut.btO184,str);
        tv1->setItem(2,0,new QTableWidgetItem(QString("O184    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceOut.btO188 != mOut.btO188)
    {
        mOut.btO188 = edm->m_stEdmShowData.stEdmInterfaceOut.btO188;
        Char2QStringInBinary(mOut.btO188,str);
        tv1->setItem(3,0,new QTableWidgetItem(QString("O188    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceOut.btO18C != mOut.btO18C)
    {
        mOut.btO18C = edm->m_stEdmShowData.stEdmInterfaceOut.btO18C;
        Char2QStringInBinary(mOut.btO18C,str);
        tv1->setItem(4,0,new QTableWidgetItem(QString("O18C    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceOut.btO190 != mOut.btO190)
    {
        mOut.btO190 = edm->m_stEdmShowData.stEdmInterfaceOut.btO190;
        Char2QStringInBinary(mOut.btO190,str);
        tv1->setItem(5,0,new QTableWidgetItem(QString("O190    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceOut.btO198 != mOut.btO198)
    {
        mOut.btO198 = edm->m_stEdmShowData.stEdmInterfaceOut.btO198;
        Char2QStringInBinary(mOut.btO198,str);
        tv1->setItem(6,0,new QTableWidgetItem(QString("O198    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceOut.btO199 != mOut.btO199)
    {
        mOut.btO199 = edm->m_stEdmShowData.stEdmInterfaceOut.btO199;
        Char2QStringInBinary(mOut.btO199,str);
        tv1->setItem(7,0,new QTableWidgetItem(QString("O199    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceOut.btO1C0 != mOut.btO1C0)
    {
        mOut.btO1C0 = edm->m_stEdmShowData.stEdmInterfaceOut.btO1C0;
        Char2QStringInBinary(mOut.btO1C0,str);
        tv1->setItem(8,0,new QTableWidgetItem(QString("O1C0    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceOut.btO1C4 != mOut.btO1C4)
    {
        mOut.btO1C4 = edm->m_stEdmShowData.stEdmInterfaceOut.btO1C4;
        Char2QStringInBinary(mOut.btO1C4,str);
        tv1->setItem(9,0,new QTableWidgetItem(QString("O1C4    %1").arg(str)));
    }

    //setIn
    if(edm->m_stEdmShowData.stEdmInterfaceIn.btI140 != mIn.btI140)
    {
        mIn.btI140 = edm->m_stEdmShowData.stEdmInterfaceIn.btI140;
        Char2QStringInBinary(mIn.btI140,str);
        tv1->setItem(0,1,new QTableWidgetItem(QString("I140    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceIn.btI144 != mIn.btI144)
    {
        mIn.btI144 = edm->m_stEdmShowData.stEdmInterfaceIn.btI144;
        Char2QStringInBinary(mIn.btI144,str);
        tv1->setItem(1,1,new QTableWidgetItem(QString("I144    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceIn.btI148 != mIn.btI148)
    {
        mIn.btI148 = edm->m_stEdmShowData.stEdmInterfaceIn.btI148;
        Char2QStringInBinary(mIn.btI148,str);
        tv1->setItem(2,1,new QTableWidgetItem(QString("I148    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceIn.btI184 != mIn.btI184)
    {
        mIn.btI184 = edm->m_stEdmShowData.stEdmInterfaceIn.btI184;
        Char2QStringInBinary(mIn.btI184,str);
        tv1->setItem(3,1,new QTableWidgetItem(QString("I184    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceIn.btI188 != mIn.btI188)
    {
        mIn.btI188 = edm->m_stEdmShowData.stEdmInterfaceIn.btI188;
        Char2QStringInBinary(mIn.btI188,str);
        tv1->setItem(4,1,new QTableWidgetItem(QString("I188    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceIn.btI1C0 != mIn.btI1C0)
    {
        mIn.btI1C0 = edm->m_stEdmShowData.stEdmInterfaceIn.btI1C0;
        Char2QStringInBinary(mIn.btI1C0,str);
        tv1->setItem(5,1,new QTableWidgetItem(QString("I1C0    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceIn.btI1C4 != mIn.btI1C4)
    {
        mIn.btI1C4 = edm->m_stEdmShowData.stEdmInterfaceIn.btI1C4;
        Char2QStringInBinary(mIn.btI1C4,str);
        tv1->setItem(6,1,new QTableWidgetItem(QString("I1C4    %1").arg(str)));
    }
    if(edm->m_stEdmShowData.stEdmInterfaceIn.btI1C8 != mIn.btI1C8)
    {
        mIn.btI1C8 = edm->m_stEdmShowData.stEdmInterfaceIn.btI1C8;
        Char2QStringInBinary(mIn.btI1C8,str);
        tv1->setItem(7,1,new QTableWidgetItem(QString("I1C8    %1").arg(str)));
    }

}
