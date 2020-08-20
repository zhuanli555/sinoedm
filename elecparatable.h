#ifndef ELECPARATABLE_H
#define ELECPARATABLE_H

#include <QTableView>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QtSql>

class ElecParaTable : public QTableView
{
    Q_OBJECT
public:
    explicit ElecParaTable(QSqlRelationalTableModel *elecpage, QSqlTableModel *elecoral, QString name, QWidget *parent = 0);
    void showData(QSqlRelationalTableModel *elecpage, QSqlTableModel *elecoral, QString name);
    ~ElecParaTable();
private:

    QSqlRelationalTableModel *elecPageModel;
    QSqlTableModel *elecOralModel;
    QStandardItemModel* _model;
    QTableView* elecPageView;

public slots:
    void dataInsertSlot();
    void dataChangedSlot(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int> ());
};

#endif // ELECPARATABLE_H
