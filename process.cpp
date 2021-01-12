#include <QDebug>
#include <QtGui/QKeyEvent>
#include <QTextCodec>
#include <QFile>
#include <QFileDialog>
#include <QSqlRecord>
#include <QTextStream>
#include <QApplication>
#include <QDesktopWidget>
#include <QtConcurrent>
#include "process.h"

//微孔机

extern QString path;
Process::Process(QWidget *parent): QMainWindow(parent)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QWidget* widget = new QWidget();
    this->setCentralWidget(widget);
    setWindowTitle(QString::fromLocal8Bit("数控机床v1.0"));
    setGeometry(0,0,QApplication::desktop()->availableGeometry().width(),QApplication::desktop()->availableGeometry().height());
    //获取edm实例 加工多线程
    EDMProcessInit();
    //状态栏
    statBar = statusBar();
    bPause = 1;
    //left
    coordWidget = new CoordWidget();
    //right
    alarmSignal = new AlarmSignal();
    fileLabel = new QLabel(QString::fromLocal8Bit("加工文件名(F4)"));
    fileText = new QPlainTextEdit;
    fileText->setReadOnly(true);
    fileText->setMaximumWidth(400);
    rightLayout = new QGridLayout();
    rightLayout->setSpacing(20);
    rightLayout->addWidget(alarmSignal,0,1,6,1);
    rightLayout->addWidget(fileLabel,0,0,1,1);
    rightLayout->addWidget(fileText,1,0,5,1);
    rightLayout->setSizeConstraint(QLayout::SetFixedSize);
    //bottom
    elecPageModel = new QSqlRelationalTableModel(this);
    elecPageModel->setTable("elec_page");
    elecPageModel->setRelation(2, QSqlRelation("elec_oral", "id", "manufactory"));
    elecPageModel->select();

    elecOralModel = new QSqlTableModel(this);
    elecOralModel->setTable("elec_oral");
    elecOralModel->select();
    elecParaTable = new ElecParaTable(elecPageModel,elecOralModel,"");
    QGridLayout *bgLayout = new QGridLayout();
    QLabel *jxProcess = new QLabel(QString::fromLocal8Bit("精铣加工"));
    QLabel *ProcessNum = new QLabel(QString::fromLocal8Bit("加工段号"));
    QLabel *singleProcess = new QLabel(QString::fromLocal8Bit("单段加工"));
    QLabel *safePlace = new QLabel(QString::fromLocal8Bit("安全位置"));
    QLabel *backPlace = new QLabel(QString::fromLocal8Bit("回退位置"));
    QLabel *smallTime = new QLabel(QString::fromLocal8Bit("最小时限"));
    QLabel *bigTime = new QLabel(QString::fromLocal8Bit("最大时限"));
    QLabel *shakeHz = new QLabel(QString::fromLocal8Bit("振动频率"));
    QLabel *bottomTime = new QLabel(QString::fromLocal8Bit("底部停留"));
    QLabel *daoZhui = new QLabel(QString::fromLocal8Bit("倒锥功能"));
    QLabel *elecLen = new QLabel(QString::fromLocal8Bit("电极余长"));
    jxProcessVal = new QLineEdit("1234.56");jxProcessVal->setMaximumWidth(80);
    ProcessNumVal = new QLineEdit("1234.56");ProcessNumVal->setMaximumWidth(80);
    singleProcessVal = new QLineEdit("1234.56");singleProcessVal->setMaximumWidth(80);
    safePlaceVal = new QLineEdit("1234.56");safePlaceVal->setMaximumWidth(80);
    backPlaceVal = new QLineEdit("1234.56");backPlaceVal->setMaximumWidth(80);
    smallTimeVal = new QLineEdit("1234.56");smallTimeVal->setMaximumWidth(80);
    bigTimeVal = new QLineEdit("1234.56");bigTimeVal->setMaximumWidth(80);
    shakeHzVal = new QLineEdit("1234.56");shakeHzVal->setMaximumWidth(80);
    bottomTimeVal = new QLineEdit("1234.56");bottomTimeVal->setMaximumWidth(80);
    daoZhuiVal = new QLineEdit("1234.56");daoZhuiVal->setMaximumWidth(80);
    elecLenVal = new QLineEdit("1234.56");elecLenVal->setMaximumWidth(80);
    bgLayout->addWidget(jxProcess,0,0);bgLayout->addWidget(jxProcessVal,0,1);
    bgLayout->addWidget(ProcessNum,1,0);bgLayout->addWidget(ProcessNumVal,1,1);
    bgLayout->addWidget(singleProcess,2,0);bgLayout->addWidget(singleProcessVal,2,1);
    bgLayout->addWidget(safePlace,3,0);bgLayout->addWidget(safePlaceVal,3,1);
    bgLayout->addWidget(backPlace,4,0);bgLayout->addWidget(backPlaceVal,4,1);
    bgLayout->addWidget(smallTime,5,0);bgLayout->addWidget(smallTimeVal,5,1);
    bgLayout->addWidget(bigTime,6,0);bgLayout->addWidget(bigTimeVal,6,1);
    bgLayout->addWidget(shakeHz,7,0);bgLayout->addWidget(shakeHzVal,7,1);
    bgLayout->addWidget(bottomTime,8,0);bgLayout->addWidget(bottomTimeVal,8,1);
    bgLayout->addWidget(daoZhui,9,0);bgLayout->addWidget(daoZhuiVal,9,1);
    bgLayout->addWidget(elecLen,10,0);bgLayout->addWidget(elecLenVal,10,1);
    QLabel *findCenter = new QLabel(QString::fromLocal8Bit("找 中 心"));
    QLabel *totalTime = new QLabel(QString::fromLocal8Bit("累计时间"));
    QLabel *processTime = new QLabel(QString::fromLocal8Bit("加工时间"));
    QLabel *st1 = new QLabel(QString::fromLocal8Bit("单孔时间"));
    QLabel *st2 = new QLabel(QString::fromLocal8Bit("单孔时间"));
    QLabel *st3 = new QLabel(QString::fromLocal8Bit("单孔时间"));
    QLabel *st4 = new QLabel(QString::fromLocal8Bit("单孔时间"));
    QLabel *st5 = new QLabel(QString::fromLocal8Bit("单孔时间"));
    QLabel *st6 = new QLabel(QString::fromLocal8Bit("单孔时间"));
    QLabel *st7 = new QLabel(QString::fromLocal8Bit("单孔时间"));
    QLabel *st8 = new QLabel(QString::fromLocal8Bit("单孔时间"));
    findCenterVal = new QLabel("1234.56");
    totalTimeVal = new QLabel("1234.56");
    processTimeVal = new QLabel("1234.56");
    st1Val = new QLabel("1234.56");
    st2Val = new QLabel("1234.56");
    st3Val = new QLabel("1234.56");
    st4Val = new QLabel("1234.56");
    st5Val = new QLabel("1234.56");
    st6Val = new QLabel("1234.56");
    st7Val = new QLabel("1234.56");
    st8Val = new QLabel("1234.56");
    bgLayout->addWidget(findCenter,0,2);bgLayout->addWidget(findCenterVal,0,3);
    bgLayout->addWidget(totalTime,1,2);bgLayout->addWidget(totalTimeVal,1,3);
    bgLayout->addWidget(processTime,2,2);bgLayout->addWidget(processTimeVal,2,3);
    bgLayout->addWidget(st1,3,2);bgLayout->addWidget(st1Val,3,3);
    bgLayout->addWidget(st2,4,2);bgLayout->addWidget(st2Val,4,3);
    bgLayout->addWidget(st3,5,2);bgLayout->addWidget(st3Val,5,3);
    bgLayout->addWidget(st4,6,2);bgLayout->addWidget(st4Val,6,3);
    bgLayout->addWidget(st5,7,2);bgLayout->addWidget(st5Val,7,3);
    bgLayout->addWidget(st6,8,2);bgLayout->addWidget(st6Val,8,3);
    bgLayout->addWidget(st7,9,2);bgLayout->addWidget(st7Val,9,3);
    bgLayout->addWidget(st8,10,2);bgLayout->addWidget(st8Val,10,3);

    QVBoxLayout *rgLayout = new QVBoxLayout();
    mainAxisUp = new QLabel(QString::fromLocal8Bit("主轴回升"));
    confirmHole = new QLabel(QString::fromLocal8Bit("确认孔位"));
    elecZero = new QLabel(QString::fromLocal8Bit("电极对零"));
    elecFix = new QLabel(QString::fromLocal8Bit("电极修整"));
    elecProcess = new QLabel(QString::fromLocal8Bit("放电加工"));

    rgLayout->addWidget(mainAxisUp);
    rgLayout->addWidget(confirmHole);
    rgLayout->addWidget(elecZero);
    rgLayout->addWidget(elecFix);
    rgLayout->addWidget(elecProcess);
    //middlelayout
    QVBoxLayout *midLayout = new QVBoxLayout();
    QHBoxLayout *mPtLayout = new QHBoxLayout();
    addButton = new QPushButton("add");
    delButton = new QPushButton("del");
    QTableView *tableView = new QTableView;
    connect(addButton,&QPushButton::clicked,this,&Process::insertRow);
    connect(delButton,&QPushButton::clicked,this,&Process::deleteRow);
    mPtLayout->addWidget(addButton);
    mPtLayout->addWidget(delButton);
    midLayout->addLayout(mPtLayout);
    //tableview
    midLayout->addWidget(tableView);

    bottomLayout = new QHBoxLayout();
    //bottomLayout->addStretch();
    bottomLayout->addLayout(bgLayout);
    bottomLayout->addLayout(midLayout);
    bottomLayout->addLayout(rgLayout);
    //main
    QGridLayout *mainLayout =new QGridLayout();
    mainLayout->setMargin(15);					//设定对话框的边距为15
    mainLayout->setSpacing(10);
    mainLayout->addWidget(coordWidget,0,0);
    mainLayout->addLayout(rightLayout,0,1);
    mainLayout->addLayout(bottomLayout,1,0,1,2);
    mainLayout->setColumnStretch(0,1);//设置1:1
    mainLayout->setColumnStretch(1,1);
    mainLayout->setRowStretch(0,1);
    mainLayout->setRowStretch(1,1);
    widget->setLayout(mainLayout);
    createActions();
    createMenus();
    //设置多线程
    QtConcurrent::run(this,&Process::MacProcessOperate);
    //设置定时器设置20ms
    QTimer *t = new QTimer(this);
    connect(t,&QTimer::timeout,this,&Process::timeUpdate);
    t->start(20);
}

Process::~Process()
{
    EDM_OP_List::DeleteEdmOpList();
}

void Process::insertRow()
{
    QModelIndex index = elecParaTable->selectionModel()->currentIndex();
    QAbstractItemModel *model = elecParaTable->model();

    if (!model->insertRow(index.row()+1, index.parent()))
        return;

    updateActions();

    for (int column = 0; column < model->columnCount(index.parent()); ++column) {
        QModelIndex child = model->index(index.row()+1, column, index.parent());
        model->setData(child, QVariant("[No data]"), Qt::EditRole);
    }
}

void Process::deleteRow()
{
    QModelIndex index = elecParaTable->selectionModel()->currentIndex();
    QAbstractItemModel *model = elecParaTable->model();
    if (model->removeRow(index.row(), index.parent()))
        updateActions();
}

void Process::updateActions()
{
    bool hasSelection = !elecParaTable->selectionModel()->selection().isEmpty();
    delButton->setEnabled(hasSelection);

    bool hasCurrent = elecParaTable->selectionModel()->currentIndex().isValid();
    addButton->setEnabled(hasCurrent);


}
void Process::createMenus()
{
    QMenuBar* myMenu = menuBar();
    myMenu->setStyleSheet("QMenuBar::item{\
      background-color:rgb(89,87,87);margin:2px 2px;color:yellow;}\
       QMenuBar::item:selected{background-color:rgb(235,110,36);}\
        QMenuBar::item:pressed{background-color:rgb(235,110,6);border:1px solid rgb(60,60,60);}");
    myMenu->addAction(processAction);
    myMenu->addAction(imitateAction);
    myMenu->addAction(pauseAction);
}

void Process::createActions()
{
    processAction = new QAction(QString::fromLocal8Bit("编程加工(F1)"),this);
    processAction->setShortcut(tr("F1"));
    processAction->setStatusTip("编程加工");
    connect(processAction,&QAction::triggered,this,&Process::programProcess);

    imitateAction = new QAction(QString::fromLocal8Bit("模拟加工(F2)"),this);
    imitateAction->setShortcut(tr("F2"));
    imitateAction->setStatusTip("模拟加工");
    connect(imitateAction,&QAction::triggered,this,&Process::imitateProcess);

    pauseAction = new QAction(QString::fromLocal8Bit("暂停(F3)"),this);
    pauseAction->setShortcut(tr("F3"));
    pauseAction->setStatusTip("暂停");
    connect(pauseAction,&QAction::triggered,this,&Process::pause);

}
bool _abort=false;
//加工线程
void Process::MacProcessOperate()
{
    while (true)
    {
        if(_abort)return;
        QMutexLocker lock(&mutex);
        if (edm)
        {
            HandleOpMsg();
            HandleEdmOpStatus();

            if (edmOpList)
            {
                qDebug()<<"edm op carry on";
                edmOpList->CarryOn();
            }
        }
        QThread::msleep(20);
    }
    EDM_OP_List::DeleteEdmOpList();
}

//处理各个类型的加工
void Process::HandleOpMsg()
{
    static string  strElec;
    static MAC_ELEC_PARA elec;
    QString strFileName;
    if (!edmOpList)
    {
        return;
    }

    switch(gMsg)
    {
    case MSG_OP:
        {
            edmOpList->DeleteEdmOp();
            edmOpList->SetEdmOpType(enType);
            edmOpList->ResetEdmOpFile();
            edmOpList->SetStart(FALSE);
            edm->CloseHardWare();
        }
        break;
    case MSG_PAUSE:
        {
            if (!edmOpList->m_pEdmOp)
                return;
//            edmOpList->m_pEdmOp->SetPassPara(m_dlgPassChart.m_fElec
//                                  ,m_fSpeed
//                                  ,m_iSpeedFilterCnt
//                                  ,m_iElecFilterCnt);//设置电参数和速度
            edmOpList->m_pEdmOp->EdmOpSetStart(FALSE);//设置停止

        }
        break;
    case MSG_FILE:
        {
        //获取filename
           edmOpList->SetEdmOpFile(path,gFilename);
        }
        break;
    case MSG_TEST:
        {
            if (!edmOpList->m_pEdmOp || !edm->m_stSysSet.stSetNoneLabel.bCycleMeasure)
                return;
            edmOpList->m_pEdmOp->EdmOpSetTest(TRUE);
            edmOpList->m_pEdmOp->EdmOpSetStart(TRUE);
        }
        break;
    case MSG_ELEC:
        {
        //获取电参数
//            strElec = *(string*)(pMsg->wParam);
//            memcpy(&elec,(MAC_ELEC_PARA*)(pMsg->lParam),sizeof(MAC_ELEC_PARA));
//            if (!edmOpList->m_pEdmOp)
//            {
//                if (((MAC_ELEC_PARA*)(pMsg->lParam))->iParaIndex >=0 && ((MAC_ELEC_PARA*)(pMsg->lParam))->iParaIndex < OP_HOLE_PAGE_MAX)
//                {
//                    edm->WriteElecPara(&(((MAC_ELEC_PARA*)(pMsg->lParam))->stElecPage[((MAC_ELEC_PARA*)(pMsg->lParam))->iParaIndex]),"HandleOpMsg");
//                }
//            }
//            else
//            {
//                edmOpList->m_pEdmOp->SetEdmOpElec(strElec,elec);
//            }
        }
        break;
    default:
        break;
    }
}

//处理加工状态
void Process::HandleEdmOpStatus()
{
    static int iCmdIndex=-1;
    static BOOL bOver=FALSE;
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

void Process::pause()
{
    if (!edmOpList->m_pEdmOp)return;
    bPause = !bPause;
    edmOpList->m_pEdmOp->EdmOpSetStart(bPause);
}

void Process::programProcess()
{
    enType = OP_HOLE_PROGRAME;
    gMsg = MSG_OP;
}

void Process::imitateProcess()
{
    enType = OP_HOLE_SIMULATE;
    gMsg = MSG_OP;
}



void Process::timeUpdate()
{

}

BOOL Process::EDMProcessInit()
{
    BOOL bInit;
    edm =  EDM::GetEdmInstance();
    bInit = edm->EdmInit();
    //edm->GetFileName(strOpName);//从数据库获取filename
    edm->m_strSysPath = path;
    strOpName = gFilename;
    edmOpList = EDM_OP_List::GetEdmOpListPtr();
    edmOp = edmOpList->m_pEdmOp;
    edmOpList->SetEdmOpFile(path,strOpName);
    //从数据库载入
    //todo
    return bInit;
}

void Process::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    if (e->key() == Qt::Key_F4){
        showFileText();
    }
    else if((e->modifiers() == Qt::AltModifier) && (e->key() == Qt::Key_X)){
    }
}

void Process::showFileText()
{
    gFilename = QFileDialog::getOpenFileName(this,"open File",path);
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
    fileLabel->setText(QString::fromLocal8Bit("加工文件名(F4):")+m_strElecName);
    //重新渲染表格
    elecParaTable->showData(elecPageModel,elecOralModel,m_strElecName);
}
