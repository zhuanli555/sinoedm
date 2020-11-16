#include "elecparatable.h"
#include <QDebug>
#include <QTextCodec>
#include <QPushButton>

ElecParaTable::ElecParaTable(QSqlRelationalTableModel *elecpage, QSqlTableModel *elecoral,QString name,QWidget *parent)
    : QTableView(parent)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    QPushButton *addButton =new QPushButton("add",this);
    //add table header
    _model = new QStandardItemModel(this);
    _model->setColumnCount(9);
    _model->setHeaderData(0,Qt::Horizontal, QString::fromLocal8Bit("眿宽"));
    _model->setHeaderData(1,Qt::Horizontal, QString::fromLocal8Bit("眿停"));
    _model->setHeaderData(2,Qt::Horizontal, QString::fromLocal8Bit("电流"));
    _model->setHeaderData(3,Qt::Horizontal, QString::fromLocal8Bit("电容"));
    _model->setHeaderData(4,Qt::Horizontal, QString::fromLocal8Bit("间隙控制"));
    _model->setHeaderData(5,Qt::Horizontal, QString::fromLocal8Bit("伺服"));
    _model->setHeaderData(6,Qt::Horizontal, QString::fromLocal8Bit("灵敏"));
    _model->setHeaderData(7,Qt::Horizontal, QString::fromLocal8Bit("修整量"));
    _model->setHeaderData(8,Qt::Horizontal, QString::fromLocal8Bit("加工量"));

    connect(_model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(dataChangedSlot(QModelIndex,QModelIndex,QVector<int>)));
    connect(addButton,SIGNAL(clicked()),this,SLOT(dataInsertSlot()));//cant connect in there
    connect(delButton,SIGNAL(clicked()),this,SLOT(dataDeleteSlot()));
    showData(elecpage,elecoral,name);
    this->setModel(_model);
}

//update
void ElecParaTable::dataChangedSlot(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    qDebug() << _model->data(topLeft).toString() << endl;
}

//add
void ElecParaTable::dataInsertSlot()
{
    int rowIndex = elecPageModel->rowCount() + 1;
    _model->setHeaderData(rowIndex,Qt::Vertical, QObject::tr("page %1").arg(rowIndex));

    _model->insertRow(0);
}

void ElecParaTable::dataDeleteSlot()
{
    //find the index and delete and remove from db
}

void ElecParaTable::showData(QSqlRelationalTableModel *elecpage, QSqlTableModel *elecoral, QString name)
{
    elecPageModel = elecpage;
    elecOralModel = elecoral;

    if(name.isEmpty())return;
    elecOralModel->setFilter(QObject::tr("filename = '%1'").arg(name)); //根据姓名进行筛选
    elecOralModel->select(); //显示结果
    if(elecOralModel->record(0).isEmpty())return;
    QString elecid = elecOralModel->record(0).value("id").toString();
    elecPageModel->setFilter(QObject::tr("elecid = %1").arg(elecid));
    elecPageModel->setSort(0,Qt::AscendingOrder);
    elecPageModel->select();

    int rowCount = elecPageModel->rowCount();
    _model->setRowCount(rowCount);
    for (int i = 0; i < rowCount; ++i)
    {
        _model->setHeaderData(i,Qt::Vertical, QObject::tr("page %1").arg(i));
        _model->setItem(i, 0, new QStandardItem(elecPageModel->record(i).value("iTon").toString()));
        _model->setItem(i, 1, new QStandardItem(elecPageModel->record(i).value("iToff").toString()));
        _model->setItem(i, 2, new QStandardItem(elecPageModel->record(i).value("iElecLow").toString()));
        _model->setItem(i, 3, new QStandardItem(elecPageModel->record(i).value("iCap").toString()));
        _model->setItem(i, 4, new QStandardItem(elecPageModel->record(i).value("iToff").toString()));
        _model->setItem(i, 5, new QStandardItem(elecPageModel->record(i).value("iServo").toString()));
        _model->setItem(i, 6, new QStandardItem(elecPageModel->record(i).value("iFeedSense").toString()));
        _model->setItem(i, 7, new QStandardItem(elecPageModel->record(i).value("iOpLen").toString()));
        _model->setItem(i, 8, new QStandardItem(elecPageModel->record(i).value("iOpLen").toString()));


    }
    this->setModel(_model);
}

ElecParaTable::~ElecParaTable()
{

}
