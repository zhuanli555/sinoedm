#include "spindelegate.h"
#include <QSpinBox>
SpinDelegate::SpinDelegate(QObject *parent):QItemDelegate(parent)
{

}
QWidget *SpinDelegate::createEditor(QWidget *parent,const QStyleOptionViewItem &/*option*/,const QModelIndex &/*index*/) const
{
    QSpinBox *editor = new QSpinBox(parent);
    editor->setRange(this->mininum,this->maxinum);
    editor->setSingleStep(this->step);
    editor->installEventFilter(const_cast<SpinDelegate*>(this));
    return editor;
}

void SpinDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
    int value =index.model()->data(index).toInt();
    QSpinBox *box = static_cast<QSpinBox*>(editor);
    box->setValue(value);
}

void SpinDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const
{
    QSpinBox *box = static_cast<QSpinBox*>(editor);
    int value = box->value();
    model->setData(index,value);
}

void SpinDelegate::updateEditorGeometry(QWidget *editor,
const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}

void SpinDelegate::setRangeStep(int mininum, int maxinum, int step)
{
    this->mininum = mininum;
    this->maxinum = maxinum;
    this->step = step;
}
