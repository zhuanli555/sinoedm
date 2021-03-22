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


Process::Process(QWidget *parent): QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QWidget* widget = new QWidget();
    this->setCentralWidget(widget);
    //状态栏
    statBar = statusBar();
    //left
    coordWidget = CoordWidget::getInstance();
    //right
    alarmSignal = AlarmSignal::getInstance();

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
}

Process::~Process()
{

}


//void Process::createMenus()
//{
//    QMenuBar* myMenu = menuBar();
//    myMenu->setStyleSheet("QMenuBar::item{\
//      background-color:rgb(89,87,87);margin:2px 2px;color:yellow;}\
//       QMenuBar::item:selected{background-color:rgb(235,110,36);}\
//        QMenuBar::item:pressed{background-color:rgb(235,110,6);border:1px solid rgb(60,60,60);}");
//    myMenu->addAction(stopAction);
//    myMenu->addAction(processAction);
//    myMenu->addAction(imitateAction);

//}

//void Process::createActions()
//{
//    stopAction = new QAction(QString::fromLocal8Bit("总停(ESC)"),this);
//    stopAction->setShortcut(tr("ESC"));
//    stopAction->setStatusTip(tr("总停"));

//    processAction = new QAction(QString::fromLocal8Bit("编程加工(F1)"),this);
//    processAction->setShortcut(tr("F1"));
//    processAction->setStatusTip("编程加工");

//    imitateAction = new QAction(QString::fromLocal8Bit("模拟加工(F2)"),this);
//    imitateAction->setShortcut(tr("F2"));
//    imitateAction->setStatusTip("模拟加工");

//}






