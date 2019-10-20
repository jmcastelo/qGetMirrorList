#include "mirrorSortFilterProxyModel.h"

MirrorSortFilterProxyModel::MirrorSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent){}

bool MirrorSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

    QString leftString = leftData.toString();
    QString rightString= rightData.toString();

    return leftString.compare(rightString, Qt::CaseSensitive) < 0;
}

QVariant MirrorSortFilterProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Vertical) {
        return QStringLiteral("%1").arg(section + 1);
    }

    return sourceModel()->headerData(section, orientation, role);
}
