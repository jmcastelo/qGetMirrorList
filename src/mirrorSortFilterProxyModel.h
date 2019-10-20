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

#ifndef MIRRORSORTFILTERPROXYMODEL_H
#define MIRRORSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class MirrorSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
        MirrorSortFilterProxyModel(QObject *parent = 0);

        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    protected:
        bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif
