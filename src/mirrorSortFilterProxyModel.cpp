// Copyright 2019 José María Castelo Ares

// This file is part of qGetMirrorList.

// qGetMirrorList is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// qGetMirrorList is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with qGetMirrorList.  If not, see <https://www.gnu.org/licenses/>.

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

    return false;
}

QVariant MirrorSortFilterProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Vertical) {
        return QStringLiteral("%1").arg(section + 1);
    }

    return sourceModel()->headerData(section, orientation, role);
}
