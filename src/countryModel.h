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

#ifndef COUNTRYMODEL_H
#define COUNTRYMODEL_H

#include "mirrorManager.h"
#include <QAbstractListModel>
#include <QVariant>
#include <QStringList>

class CountryModel : public QAbstractListModel
{
    Q_OBJECT

    public:
        CountryModel(QObject *parent = 0);
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    public slots:
        void getCountryList();

    private slots:
        void setCountryList(QList<Country> cl);

    private:
        QList<Country> countryList;
        MirrorManager theMirrorManager;
};

#endif
