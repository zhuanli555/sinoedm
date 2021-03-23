#include "mainwindow.h"
#include "EDM/initdb.h"
#include <QDialog>
#include <QtGui/QKeyEvent>
#include <QTimer>
#include <QDateTime>
#include <QGridLayout>
#include <QDebug>
#include <QTextCodec>
#include <QFile>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopWidget>
#include <QtConcurrent>

extern QString path;
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
    statBar = this->statusBar();
    //left
    coordWidget = CoordWidget::getInstance();
    //right
    alarmSignal = AlarmSignal::getInstance();
    fileLabel = new QLabel(QString::fromLocal8Bit("加工文件名(F11)"));
    fileText = new QPlainTextEdit;
    fileText->setReadOnly(true);
    fileText->setMaximumWidth(400);
    rightLayout = new QGridLayout;
    rightLayout->setSpacing(20);
    rightLayout->addWidget(fileLabel,0,0,1,1);
    rightLayout->addWidget(fileText,1,0,5,1);
    rightLayout->addWidget(alarmSignal,0,1,6,1);
    rightLayout->setSizeConstraint(QLayout::SetFixedSize);
    //tab
    tab = new QTabWidget;
    tab->addTab(createCommandTab(),QString::fromLocal8Bit("发送命令"));
    tab->addTab(createProcessTab(),QString::fromLocal8Bit("加工页面"));
    //main
    QGridLayout *mainLayout =new QGridLayout;
    mainLayout->setSpacing(10);
    mainLayout->addWidget(coordWidget,0,0);
    mainLayout->addLayout(rightLayout,0,1);

    //debug
    tv1 = new QTableWidget;
    tv1->setMaximumWidth(0);

    mainLayout->addWidget(tab,1,0,1,2);
    mainLayout->addWidget(tv1,1,1);

    mainLayout->setColumnStretch(0,1);
    mainLayout->setColumnStretch(1,1);
    widget->setLayout(mainLayout);
    createActions();
    createMenus();
    //多线程中不能操作gui，使用信号槽机制
    connect(this,&MainWindow::coordWidgetChanged,coordWidget,&CoordWidget::HandleEdmCycleData);
    //设置多线程信号
    macUserHandle = QtConcurrent::run(this,&MainWindow::MacUserOperate);
    macProcessHandle = QtConcurrent::run(this,&MainWindow::MacProcessOperate);
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
    macProcessHandle.waitForFinished();
}

void MainWindow::MacUserOperate()
{
    while(!m_quit)
    {
        if(edm)
        {
            mutex.lock();
            edm->GetEdmComm();
            edm->GetEdmStatusData();
            emit coordWidgetChanged();//机床命令周期性处理
            edm->EdmAxisAdjust();
            edm->EdmAxisAdjustCircle();

            alarmSignal->EdmStatusSignChange();//机床信号周期性处理
            alarmSignal->edmHandProcess();//处理手盒
            mutex.unlock();
        }
        QThread::msleep(35);
    }
}

//加工线程
void MainWindow::MacProcessOperate()
{
    while (!m_quit)
    {
        if (edm)
        {
            mutex.lock();
            HandleEdmOpStatus();
            if (edmOpList)
            {
                edmOpList->CarryOn();
            }
            mutex.unlock();
        }
        QThread::msleep(20);
    }
    EDM_OP_List::DeleteEdmOpList();
}

unsigned char MainWindow::EDMMacInit()
{
    unsigned char bInit;
    edm =  EDM::GetEdmInstance();
    bInit = edm->EdmInit();
    edm->GetMacPara(&m_stSysSet);
    edmOpList = EDM_OP_List::GetEdmOpListPtr();
    edmOp = edmOpList->m_pEdmOp;

    QString strPath = QDir::currentPath()+"/processFile";
    QString strOpName="DEFAULT";
    edmOpList->SetEdmOpFile(strPath,strOpName);
    //从数据库载入
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
    processAction->setStatusTip(tr("加工"));
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

QWidget* MainWindow::createCommandTab()
{
    QWidget* widget = new QWidget();
    //添加一个lineedit 记录发送的命令
    commandText = new QTextEdit;
    commandLabel = new QLabel(QString::fromLocal8Bit("命令"));
    commandLine = new QLineEdit;
    connect(commandLine,&QLineEdit::returnPressed,this,&MainWindow::edmSendComand);
    QLabel *speedLabel = new QLabel(QString::fromLocal8Bit("定位速度:"));
    speedValue = new QComboBox();
    speedValue->clear();
    QStringList speedlist;
    speedlist<<"60"<<"120"<<"300"<<"600";
    speedValue->addItems(speedlist);
    QVBoxLayout* mainLayout = new QVBoxLayout;
    QHBoxLayout* bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(commandLabel);
    bottomLayout->addWidget(commandLine);
    bottomLayout->addWidget(speedLabel);
    bottomLayout->addWidget(speedValue);
    mainLayout->addWidget(commandText);
    mainLayout->addLayout(bottomLayout);
    widget->setLayout(mainLayout);
    return widget;
}

QWidget* MainWindow::createProcessTab()
{
    QWidget* widget = new QWidget();

    return widget;
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

//模拟加工
void MainWindow::renderToProcess()
{
    tab->setCurrentIndex(1);
    edmOpList->DeleteEdmOp();
    edmOpList->SetEdmOpType(OP_HOLE_SIMULATE);
    edmOpList->ResetEdmOpFile();
    edmOpList->SetStart(FALSE);
    edm->CloseHardWare();
}

void MainWindow::refresh()
{
    this->show();
}

void MainWindow::renderToProgram()
{
    program = new Program();
    program->show();
}

void MainWindow::renderToSetting()
{
    setting = new SettingDialog();
    connect(setting,&SettingDialog::systemSetChanged,this,&MainWindow::systemSetChangeForCoord);
    int res = setting->exec();
    if(res != QDialog::Accepted)
    {
        disconnect(setting,&SettingDialog::systemSetChanged,this,&MainWindow::systemSetChangeForCoord);
        return;
    }
    delete setting;
}

void MainWindow::systemSetChangeForCoord()
{
    coordWidget->update();
}

void MainWindow::setAxisValue(int label,QString str)
{
    coordWidget->setAxisValue(label,str);
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
    connect(unionZero,&UnionZero::setAxisSig,this,&MainWindow::setAxisValue);
    int res = unionZero->exec();
    if(res != QDialog::Accepted)
    {
        disconnect(unionZero,&UnionZero::setAxisSig,this,&MainWindow::setAxisValue);
        return;
    }
    delete unionZero;
}

void MainWindow::edmStop()
{
    alarmSignal->edmStop();
}

void MainWindow::showFileText()
{
    QString gFilename = QFileDialog::getOpenFileName(this,"open File",path,"*", nullptr,QFileDialog::DontUseNativeDialog);
    QFileInfo info(gFilename);
    m_strElecName = info.fileName();
    if (!gFilename.isEmpty())
    {
        QFile file(gFilename);
        QTextStream textStream;
        if (file.open(QIODevice::ReadOnly))
        {
            textStream.setDevice(&file);
            while(!textStream.atEnd())
            {
                fileText->setPlainText(textStream.readAll());
            }
        }
        file.close();
    }
    //show
    fileLabel->setText(QString::fromLocal8Bit("加工文件名(F10):")+m_strElecName);
    //重新渲染表格
    //elecParaTable->showData(elecPageModel,elecOralModel,m_strElecName);
}


//处理各个类型的加工
//void MainWindow::HandleOpMsg()
//{
//    static string  strElec;
//    static MAC_ELEC_PARA elec;
//    QString strFileName;
//    if (!edmOpList)
//    {
//        return;
//    }

//    switch(gMsg)
//    {
//    case MSG_OP:
//        {
//            edmOpList->DeleteEdmOp();
//            edmOpList->SetEdmOpType(enType);
//            edmOpList->ResetEdmOpFile();
//            edmOpList->SetStart(FALSE);
//            edm->CloseHardWare();
//        }
//        break;
//    case MSG_PAUSE:
//        {
//            if (!edmOpList->m_pEdmOp)
//                return;
////            edmOpList->m_pEdmOp->SetPassPara(m_dlgPassChart.m_fElec
////                                  ,m_fSpeed
////                                  ,m_iSpeedFilterCnt
////                                  ,m_iElecFilterCnt);//设置电参数和速度
//            edmOpList->m_pEdmOp->EdmOpSetStart(FALSE);//设置停止

//        }
//        break;
//    case MSG_FILE:
//        {
//        //获取filename
//           edmOpList->SetEdmOpFile(path,m_strElecName);
//        }
//        break;
//    case MSG_TEST:
//        {
//            if (!edmOpList->m_pEdmOp || !edm->m_stSysSet.stSetNoneLabel.bCycleMeasure)
//                return;
//            edmOpList->m_pEdmOp->EdmOpSetTest(TRUE);
//            edmOpList->m_pEdmOp->EdmOpSetStart(TRUE);
//        }
//        break;
//    case MSG_ELEC:
//        {
//        //获取电参数
////            strElec = *(string*)(pMsg->wParam);
////            memcpy(&elec,(MAC_ELEC_PARA*)(pMsg->lParam),sizeof(MAC_ELEC_PARA));
////            if (!edmOpList->m_pEdmOp)
////            {
////                if (((MAC_ELEC_PARA*)(pMsg->lParam))->iParaIndex >=0 && ((MAC_ELEC_PARA*)(pMsg->lParam))->iParaIndex < OP_HOLE_PAGE_MAX)
////                {
////                    edm->WriteElecPara(&(((MAC_ELEC_PARA*)(pMsg->lParam))->stElecPage[((MAC_ELEC_PARA*)(pMsg->lParam))->iParaIndex]),"HandleOpMsg");
////                }
////            }
////            else
////            {
////                edmOpList->m_pEdmOp->SetEdmOpElec(strElec,elec);
////            }
//        }
//        break;
//    default:
//        break;
//    }
//}
//处理加工状态
void MainWindow::HandleEdmOpStatus()
{
    static int iCmdIndex=-1;
    static unsigned char bOver=FALSE;
    static OP_ERROR op_error = OP_NO_ERR;
    static vector<QString> vCmd;
    static MAP_ELEC_MAN mpElec;

    EDM_OP* pOp;

    if (!edmOpList)
    {
        return;
    }

    pOp = edmOpList->m_pEdmOp;
    if (pOp != edmOp || edmOpList->m_bChange)
    {
        edmOp = pOp;
        edmOpList->m_bChange = FALSE;
    }

    if (pOp->m_stOpStatus.stCycle.bPauseCmd)
    {
        pOp->m_stOpStatus.stCycle.bPauseCmd = FALSE;
    }

    if(pOp->m_stOpStatus.iCmdIndex != iCmdIndex && EDM_OP::m_bStartCount)
    {
        iCmdIndex = pOp->m_stOpStatus.iCmdIndex;
    }

    if (pOp->m_stOpStatus.stCycle.stPassChart.bClear)
    {
        pOp->m_stOpStatus.stCycle.stPassChart.bClear = FALSE;
    }
    else
    {
        if (pOp->m_stOpStatus.stCycle.stPassChart.bSet)
        {
            pOp->m_stOpStatus.stCycle.stPassChart.bSet = FALSE;
        }
        else
        {
            if (pOp->m_stOpStatus.stCycle.stPassChart.bRealTimeIn)
            {
                pOp->m_stOpStatus.stCycle.stPassChart.bRealTimeIn = FALSE;
            }
        }
    }


    if (pOp->m_stOpStatus.enErrAll.errOp != op_error )
    {
        op_error = pOp->m_stOpStatus.enErrAll.errOp;
        edm->EdmYellowLump( !(pOp->m_stOpStatus.enErrAll.errOp==OP_NO_ERR));
    }

//    m_dlgOpStatus.SetOpStatusPara(pOp->m_stOpStatus.iCmdIndex
//                                  ,pOp->m_stOpStatus.stCycle.iCycleIndex
//                                  ,pOp->m_stOpStatus.stCycle.iTimeSec
//                                  ,pOp->m_stOpStatus.stCycle.iOpPage);//旋转轴

    if (pOp->m_stOpStatus.bCheck_C_Over)
    {
        pOp->m_stOpStatus.bCheck_C_Over = FALSE;
    }

    if (bOver != edmOpList->m_bOver)
    {
        bOver = edmOpList->m_bOver;
        if (bOver)
        {
            iCmdIndex = -1;
        }
    }
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
    cmdDefault.iFreq = speed*100/6;
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
