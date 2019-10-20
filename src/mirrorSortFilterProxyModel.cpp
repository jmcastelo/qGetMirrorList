#include "mirrorSortFilterProxyModel.h"
#include <QDateTime>

MirrorSortFilterProxyModel::MirrorSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent){}

bool MirrorSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

    if (leftData.type() == QVariant::DateTime) {
        return leftData.toDateTime() < rightData.toDateTime();
    } else if (leftData.type() == QVariant::String) {
        return leftData.toString().compare(rightData.toString(), Qt::CaseSensitive) < 0;
    } else if (leftData.type() == QVariant::Double) {
        return leftData.toDouble() < rightData.toDouble();
    }
}

QVariant MirrorSortFilterProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Vertical) {
        return QStringLiteral("%1").arg(section + 1);
    }

    return sourceModel()->headerData(section, orientation, role);
}
