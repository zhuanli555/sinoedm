#ifndef SPINDELEGATE_H
#define SPINDELEGATE_H

#include <QItemDelegate>
class SpinDelegate : public QItemDelegate
{
    Q_OBJECT
private:
    int mininum;
    int maxinum;
    int step;
public:
    SpinDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex  &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setRangeStep(int mininum = 0,int maxinum = 100,int step = 1);
};

#endif // SPINDELEGATE_H
