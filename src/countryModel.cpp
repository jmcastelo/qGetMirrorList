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

#include "countryModel.h"

CountryModel::CountryModel(QObject *parent) : QAbstractListModel(parent)
{
    connect(&theMirrorManager, &MirrorManager::countryListReady, this, &CountryModel::setCountryList);
}

void CountryModel::getCountryList()
{
    theMirrorManager.getCountryList();
}

void CountryModel::setCountryList(QList<Country> cl)
{
    beginResetModel();
    countryList = cl;
    endResetModel();
}

int CountryModel::rowCount(const QModelIndex &parent) const
{
    return countryList.size();
}

QVariant CountryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= countryList.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        return countryList.at(index.row()).name;
    } else if (role == Qt::DecorationRole) {
        return countryList.at(index.row()).flag;
    } else {
        return QVariant();
    }
}

QVariant CountryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        return QString("Country");
    } else {
        return QStringLiteral("%1").arg(section + 1);
    }
}
