#include "elecpagemodel.h"

ElecPageModel::ElecPageModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant ElecPageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

bool ElecPageModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}


int ElecPageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant ElecPageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch(role)
    {
    case Qt::CheckStateRole:
        if(index.column() == 0)
        {
            if (checkBoxStatus.contains(index.row()))
                return check_state_map[index.row()] == Qt::Checked ? Qt::Checked : Qt::Unchecked;

            return Qt::Unchecked;
        }
    default:
        return QVariant();
    }
    return QVariant();
}

bool ElecPageModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags ElecPageModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.column() == 0)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;

    return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool ElecPageModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool ElecPageModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}

