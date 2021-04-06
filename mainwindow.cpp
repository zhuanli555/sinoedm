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
    coordWidget = new CoordWidget;
    //right
    alarmSignal = new AlarmSignal;
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
    //设置多线程信号
    macUserHandle = QtConcurrent::run(this,&MainWindow::MacUserOperate);
    macProcessHandle = QtConcurrent::run(this,&MainWindow::MacProcessOperate);
    //设置定时器
    QTimer *t = new QTimer(this);
    connect(t,&QTimer::timeout,this,&MainWindow::timeUpdate);
    t->start(1000);
    //多线程中不能操作gui，使用信号槽机制
    connect(this,&MainWindow::coordWidgetChanged,coordWidget,&CoordWidget::HandleEdmCycleData);
    //connect(this,&MainWindow::systemSetChangeSig,coordWidget,&CoordWidget::setLabels);
    connect(this,&MainWindow::setAxisValueSig,coordWidget,&CoordWidget::setAxisValue);
    connect(this,&MainWindow::edmPauseSig,alarmSignal,&AlarmSignal::edmPause);
    connect(this,&MainWindow::edmShakeSig,alarmSignal,&AlarmSignal::edmShake);
    connect(this,&MainWindow::edmPurgeSig,alarmSignal,&AlarmSignal::edmPurge);
    connect(this,&MainWindow::edmStopSig,alarmSignal,&AlarmSignal::edmStop);
    connect(this,&MainWindow::edmCloseSig,edm,&EDM::CloseHardWare);
    connect(this,&MainWindow::edmMoveParaSendSig,edm,&EDM::EdmSendMovePara);
    connect(this,&MainWindow::edmWriteElecSig,edm,&EDM::WriteElecPara);
    connect(this,&MainWindow::edmOPSig,edmOpList,&EDM_OP_List::CarryOnBefore);
    connect(this,&MainWindow::edmOpFileSig,edmOpList,&EDM_OP_List::SetEdmOpFile);
    connect(this,&MainWindow::edmOpElecSig,edmOpList,&EDM_OP_List::SetEdmOpElec);
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
            edm->GetEdmStatusData();
            emit coordWidgetChanged();//机床命令周期性处理
            //edm->EdmAxisAdjust();
            //edm->EdmAxisAdjustCircle();

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

    m_strElecName="DEFAULT";
    emit edmOpFileSig(path,m_strElecName);
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

    simulateAction = new QAction(QString::fromLocal8Bit("模拟加工(F9)"),this);
    simulateAction->setShortcut(tr("F9"));
    simulateAction->setStatusTip(tr("模拟加工"));
    connect(simulateAction,&QAction::triggered,this,&MainWindow::renderToSimulate);

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
    myMenu->addAction(simulateAction);
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
    elecPageTable = new QTableWidget;
    elecPageTable->setColumnCount(11);elecPageTable->setRowCount(6);
    elecOralTable = new QTableWidget;
    elecOralTable->setColumnCount(14);elecOralTable->setRowCount(1);
    int row,col=0;
    elecPageTable->setHorizontalHeaderItem(0,new QTableWidgetItem(QString::fromLocal8Bit("加工深度")));
    elecPageTable->setHorizontalHeaderItem(1,new QTableWidgetItem(QString::fromLocal8Bit("脉冲宽度ns")));
    elecPageTable->setHorizontalHeaderItem(2,new QTableWidgetItem(QString::fromLocal8Bit("脉冲停息ns")));
    elecPageTable->setHorizontalHeaderItem(3,new QTableWidgetItem(QString::fromLocal8Bit("加工电流")));
    elecPageTable->setHorizontalHeaderItem(4,new QTableWidgetItem(QString::fromLocal8Bit("低压电流")));
    elecPageTable->setHorizontalHeaderItem(5,new QTableWidgetItem(QString::fromLocal8Bit("加工电容")));
    elecPageTable->setHorizontalHeaderItem(6,new QTableWidgetItem(QString::fromLocal8Bit("伺服给定")));
    elecPageTable->setHorizontalHeaderItem(7,new QTableWidgetItem(QString::fromLocal8Bit("进给灵敏")));
    elecPageTable->setHorizontalHeaderItem(8,new QTableWidgetItem(QString::fromLocal8Bit("回退灵敏")));
    elecPageTable->setHorizontalHeaderItem(9,new QTableWidgetItem(QString::fromLocal8Bit("振动强度")));
    elecPageTable->setHorizontalHeaderItem(10,new QTableWidgetItem(QString::fromLocal8Bit("振动灵敏")));
    QStringList list;
    list<<QString::fromLocal8Bit("修电极")<<QString::fromLocal8Bit("加工页1")\
       <<QString::fromLocal8Bit("加工页2")<<QString::fromLocal8Bit("加工页3")\
      <<QString::fromLocal8Bit("加工页4")<<QString::fromLocal8Bit("加工页5");
    elecPageTable->setVerticalHeaderLabels(list);
    for(row =0;row<OP_HOLE_PAGE_MAX;row++)
    {
        for(col=0;col<11;col++)
        {
            elecPageTable->setItem(row,col,new QTableWidgetItem("0"));
        }
    }
    list.clear();
    list<<QString::fromLocal8Bit("加工行号")<<QString::fromLocal8Bit("总行数")\
       <<QString::fromLocal8Bit("回升位置")<<QString::fromLocal8Bit("安全位置")\
      <<QString::fromLocal8Bit("旋转有效")<<QString::fromLocal8Bit("连续加工")\
     <<QString::fromLocal8Bit("总深度")<<QString::fromLocal8Bit("起判位置")\
    <<QString::fromLocal8Bit("底部停歇")<<QString::fromLocal8Bit("重复次数")\
    <<QString::fromLocal8Bit("重复长度")<<QString::fromLocal8Bit("铣削伺服")\
    <<QString::fromLocal8Bit("最小时限")<<QString::fromLocal8Bit("最大时限");
    elecOralTable->setHorizontalHeaderLabels(list);
    for(col = 0;col<14;col++)
    {
        elecOralTable->setItem(0,col,new QTableWidgetItem("0"));
    }
    OpFileCopyAndSend();
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(elecPageTable);
    mainLayout->addWidget(elecOralTable);
    mainLayout->setStretchFactor(elecPageTable,3);
    mainLayout->setStretchFactor(elecOralTable,1);
    widget->setLayout(mainLayout);
    connect(elecPageTable,&QTableWidget::itemChanged,this,&MainWindow::elecTableChanged);
    connect(elecOralTable,&QTableWidget::itemChanged,this,&MainWindow::elecTableChanged);
    return widget;
}

void MainWindow::elecTableChanged()
{
    MAC_ELEC_PARA elec;
    elecOralTable->blockSignals(true);
    ReadParaFromTable(&elec);
    if (!edmOpList->m_pEdmOp)
    {
        if (elec.iParaIndex >=0 && elec.iParaIndex < OP_HOLE_PAGE_MAX)
        {
            emit edmWriteElecSig(&elec.stElecPage[elec.iParaIndex],"");
        }
    }else{
        emit edmOpElecSig(m_strElecName,elec);

    }
    elecOralTable->blockSignals(false);
}

void MainWindow::fillTableWidget(MAC_ELEC_PARA* pPara)
{
    int col,row;
    QString str;
    for (row=0;row<OP_HOLE_PAGE_MAX;row++)
    {
        col = 0;
        str = QString("%1").arg(pPara->stElecPage[row].iOpLen);
        elecPageTable->item(row,col)->setText(str);
        str = QString("%1").arg(pPara->stElecPage[row].iTon);
        elecPageTable->item(row,++col)->setText(str);
        str = QString("%1").arg(pPara->stElecPage[row].iToff);
        elecPageTable->item(row,++col)->setText(str);
        str = QString("%1").arg(pPara->stElecPage[row].iElecLow);
        elecPageTable->item(row,++col)->setText(str);
        str = QString("%1").arg(pPara->stElecPage[row].iElecHigh);
        elecPageTable->item(row,++col)->setText(str);
        str = QString("%1").arg(pPara->stElecPage[row].iCap);
        elecPageTable->item(row,++col)->setText(str);
        str = QString("%1%").arg(pPara->stElecPage[row].iServo);
        elecPageTable->item(row,++col)->setText(str);
        str = QString("%1%").arg(pPara->stElecPage[row].iFeedSense);
        elecPageTable->item(row,++col)->setText(str);
        str = QString("%1%").arg(pPara->stElecPage[row].iBackSense);
        elecPageTable->item(row,++col)->setText(str);
        str = QString("%1").arg(pPara->stElecPage[row].iShake);
        elecPageTable->item(row,++col)->setText(str);
        str = QString("%1%").arg(pPara->stElecPage[row].iShakeSense);
        elecPageTable->item(row,++col)->setText(str);
    }

    row =0;
    col = -1;
    str = QString("%1").arg(pPara->stElecOral.iOpHoleIndex);
    elecOralTable->item(row,++col)->setText(str);
    str = QString("%1").arg(pPara->stElecOral.iOpHoleAll);
    elecOralTable->item(row,++col)->setText(str);
    str = QString("%1").arg(pPara->stElecOral.iRisePos);
    elecOralTable->item(row,++col)->setText(str);
    str = QString("%1").arg(pPara->stElecOral.iSafePos);
    elecOralTable->item(row,++col)->setText(str);
    str = QString("%1").arg(pPara->stElecOral.bRotateValidate);
    elecOralTable->item(row,++col)->setText(str);
    str = QString("%1").arg(pPara->stElecOral.bContinueOp);
    elecOralTable->item(row,++col)->setText(str);
    str = QString("%1").arg(pPara->stElecOral.iOpLenAll);
    elecOralTable->item(row,++col)->setText(str);
    str = QString("%1").arg(pPara->stElecOral.iJudgePos);
    elecOralTable->item(row,++col)->setText(str);
    str = QString("%1").arg(pPara->stElecOral.iBottomSleep);
    elecOralTable->item(row,++col)->setText(str);
    str = QString("%1").arg(pPara->stElecOral.iRepeatCount);
    elecOralTable->item(row,++col)->setText(str);
    str = QString("%1").arg(pPara->stElecOral.iRepeatLen);
    elecOralTable->item(row,++col)->setText(str);
    str = QString("%1").arg(pPara->stElecOral.iMillServo);
    elecOralTable->item(row,++col)->setText(str);
    str = QString("%1").arg(pPara->stElecOral.iTimeMin);
    elecOralTable->item(row,++col)->setText(str);
    str = QString("%1").arg(pPara->stElecOral.iTimeMax);
    elecOralTable->item(row,++col)->setText(str);
}

void MainWindow::ReadParaFromTable(MAC_ELEC_PARA* pPara)
{
    int row=1;
    int col=1;
    QString str;
    pPara->iParaIndex = elecPageTable->currentRow()==-1?0:elecPageTable->currentRow();
    for (row=0;row<OP_HOLE_PAGE_MAX;row++)
    {
        col =0;
        str = elecPageTable->item(row,col)->text();
        if (str.contains('.'))
            pPara->stElecPage[row].iOpLen = (int)(str.toFloat()*1000.0);
        else
            pPara->stElecPage[row].iOpLen = str.toInt();

        str = elecPageTable->item(row,++col)->text();
        pPara->stElecPage[row].iTon = str.toInt();

        str = elecPageTable->item(row,++col)->text();
        pPara->stElecPage[row].iToff = str.toInt();

        str = elecPageTable->item(row,++col)->text();
        pPara->stElecPage[row].iElecLow = str.toInt();

        str = elecPageTable->item(row,++col)->text();
        pPara->stElecPage[row].iElecHigh = str.toInt();

        str = elecPageTable->item(row,++col)->text();
        pPara->stElecPage[row].iCap = str.toInt();

        str = elecPageTable->item(row,++col)->text();
        pPara->stElecPage[row].iServo = PercentStr2int(str);

        str = elecPageTable->item(row,++col)->text();
        pPara->stElecPage[row].iFeedSense = PercentStr2int(str);

        str = elecPageTable->item(row,++col)->text();
        pPara->stElecPage[row].iBackSense = PercentStr2int(str);

        str = elecPageTable->item(row,++col)->text();
        pPara->stElecPage[row].iShake = str.toInt();

        str = elecPageTable->item(row,++col)->text();
        pPara->stElecPage[row].iShakeSense = PercentStr2int(str);
    }


    row =0;
    col =-1;

    str = elecOralTable->item(row,++col)->text();
    pPara->stElecOral.iOpHoleIndex = str.toInt();
    col++;
    str = elecOralTable->item(row,++col)->text();
    if (str.contains('.'))
        pPara->stElecOral.iRisePos = (int)(str.toFloat()*1000.0);
    else
        pPara->stElecOral.iRisePos = str.toInt();

    str = elecOralTable->item(row,++col)->text();
    if (str.contains('.'))
        pPara->stElecOral.iSafePos = (int)(str.toFloat()*1000.0);
    else
        pPara->stElecOral.iSafePos = str.toInt();

    str = elecOralTable->item(row,++col)->text();
    pPara->stElecOral.bRotateValidate = str.toInt();

    str = elecOralTable->item(row,++col)->text();
    pPara->stElecOral.bContinueOp = str.toInt();
    col++;

    str = elecOralTable->item(row,++col)->text();
    pPara->stElecOral.iJudgePos = str.toInt();

    str = elecOralTable->item(row,++col)->text();
    pPara->stElecOral.iBottomSleep = str.toInt();

    str = elecOralTable->item(row,++col)->text();
    pPara->stElecOral.iRepeatCount = str.toInt();

    str = elecOralTable->item(row,++col)->text();
    pPara->stElecOral.iRepeatLen = str.toInt();

    str = elecOralTable->item(row,++col)->text();
    pPara->stElecOral.iMillServo = str.toInt();

    str = elecOralTable->item(row,++col)->text();
    pPara->stElecOral.iTimeMin = str.toInt();

    str = elecOralTable->item(row,++col)->text();
    pPara->stElecOral.iTimeMax = str.toInt();

    LawOfPara(pPara);

}

int MainWindow::PercentStr2int(QString str)
{
    QString strTmp=str;
    if(strTmp.contains('.'))
    {
        return strTmp.toFloat()*100;
    }
    int index = str.indexOf('%');
    if(index>=0)
    {
        strTmp = str.left(index);

    }
    return strTmp.toInt();
}

void MainWindow::LawOfPara(MAC_ELEC_PARA* pPara)
{
    for (int i=0;i<OP_HOLE_PAGE_MAX;i++)
    {
        LawInt(pPara->stElecPage[i].iOpLen,0,1000000);
        LawInt(pPara->stElecPage[i].iServo,0,100);
        LawInt(pPara->stElecPage[i].iFeedSense,0,100);
        LawInt(pPara->stElecPage[i].iBackSense,0,1000000);
        LawInt(pPara->stElecPage[i].iShake,300,800);
        LawInt(pPara->stElecPage[i].iShakeSense,0,100);
    }

    LawInt(pPara->stElecOral.iOpHoleIndex,1,pPara->stElecOral.iOpHoleAll);
    if (pPara->stElecOral.iTimeMin<0)
        pPara->stElecOral.iTimeMin = 0;
    if (pPara->stElecOral.iTimeMax<0)
        pPara->stElecOral.iTimeMax = 0;
    if (pPara->stElecOral.iBottomSleep<0)
        pPara->stElecOral.iBottomSleep = 0;
    if (pPara->stElecOral.iRepeatCount<0)
        pPara->stElecOral.iRepeatCount = 0;
    if (pPara->stElecOral.iRepeatLen<0)
        pPara->stElecOral.iRepeatLen = 0;
    LawInt(pPara->stElecOral.iMillServo,1,1000);

    pPara->stElecOral.iOpLenAll = 0;
    for (int i=0;i<OP_HOLE_PAGE_MAX;i++)
    {
        pPara->stElecOral.iOpLenAll += pPara->stElecPage[i].iOpLen;
    }
}

void MainWindow::LawInt(int& t,int low,int high)
{
    if (low >=high)
        return;

    if (t<low)
        t=low;
    else if (t>high)
        t=high;
}


void MainWindow::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Escape:
        edmStop();break;
    case Qt::Key_F4:
        emit edmPurgeSig();break;
    case Qt::Key_F5:
        alarmSignal->edmLowerPump();break;
    case Qt::Key_F6:
        emit edmShakeSig();break;
    case Qt::Key_F7:
        alarmSignal->edmProtect();break;
    case Qt::Key_F8:
        emit edmPauseSig();break;
    case Qt::Key_F11:
        showFileText();break;
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
void MainWindow::renderToSimulate()
{
    showFileText();
    tab->setCurrentIndex(1);
    emit edmOPSig(OP_HOLE_SIMULATE);
    if(!alarmSignal->bPause)
    {
        emit edmPauseSig();
    }
    emit edmCloseSig();
}

//放电加工
void MainWindow::renderToProcess()
{
    showFileText();
    tab->setCurrentIndex(1);
    emit edmOPSig(OP_HOLE_PROGRAME);
    if(!alarmSignal->bPause)
    {
        emit edmPauseSig();
    }
    emit edmCloseSig();
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
    if(res != QDialog::Accepted)return;
    delete setting;
}

void MainWindow::systemSetChangeForCoord()
{
    //emit systemSetChangeSig();
}

void MainWindow::setAxisValue(int label,QString str)
{
    emit setAxisValueSig(label,str);
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
    if(res != QDialog::Accepted)return;
    delete unionZero;
}

void MainWindow::edmStop()
{
    emit edmStopSig();
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
    fileLabel->setText(QString::fromLocal8Bit("加工文件名(F11):")+m_strElecName);
    emit edmOpFileSig(path,m_strElecName);
    //重新渲染表格
    OpFileCopyAndSend();
}

//处理加工状态
void MainWindow::HandleEdmOpStatus()
{
    static int iCmdIndex=-1;
    static unsigned char bOver=FALSE;
    static OP_ERROR op_error = OP_NO_ERR;

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
        if(!alarmSignal->bPause)
        {
            emit edmPauseSig();
        }
    }

    if(pOp->m_stOpStatus.iCmdIndex != iCmdIndex && EDM_OP::m_bStartCount)
    {
        iCmdIndex = pOp->m_stOpStatus.iCmdIndex;
    }
    if (pOp->m_stOpStatus.errOp != op_error )
    {
        op_error = pOp->m_stOpStatus.errOp;
        edm->EdmYellowLump( !(pOp->m_stOpStatus.errOp==OP_NO_ERR));
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

void MainWindow::OpFileCopyAndSend()
{
    static vector<QString> vCmd;
    static vector<QString> vCmdAbs;
    static QString strFileName;
    static MAP_ELEC_MAN mpElec;

    if (edmOpList && edmOpList->m_pEdmOp)
    {
        edmOpList->GetOpFileInfo(strFileName,&mpElec,vCmd,vCmdAbs);
        fillTableWidget(&mpElec[strFileName]);
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
    emit edmMoveParaSendSig(&stDigitCmd);
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
