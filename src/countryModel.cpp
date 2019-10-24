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
#include <algorithm>

bool comparator(Country &c1, Country &c2)
{
    return c1.name.compare(c2.name) < 0;
}

CountryModel::CountryModel(QObject *parent) : QAbstractListModel(parent) {}

void CountryModel::sortCountryList()
{
    std::sort(countryList.begin(), countryList.end(), comparator);
}

void CountryModel::setCountryList(QList<Country> cl)
{
    countryList = cl;

    beginResetModel();
    sortCountryList();
    endResetModel();
}

int CountryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
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

    // Select country name or flag according to role
    if (role == Qt::DisplayRole) {
        return countryList.at(index.row()).name;
    } else if (role == Qt::DecorationRole) {
        return countryList.at(index.row()).flag;
    } else {
        return QVariant();
    }
}

// Headers do not seem to apply to a list view. This method exists, anyway.
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
