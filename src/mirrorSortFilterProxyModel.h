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
#include <QMimeData>

struct MirrorFilter
{
    QStringList countryList;
    QStringList protocolList;
    int active;
    int isos;
    int ipv4;
    int ipv6;
};

class MirrorDnDProxyModel;

class MirrorSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    friend class MirrorDnDProxyModel;

    public:
        MirrorSortFilterProxyModel(QObject *parent = nullptr);

        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

        MirrorFilter filter;

        void appendCountryFilter(QString country);
        void removeCountryFilter(QString country);
        void appendProtocolFilter(QString protocol);
        void removeProtocolFilter(QString protocol);
        void setActiveFilter(int active);
        void setIsosFilter(int isos);
        void setIPv4Filter(int ipv4);
        void setIPv6Filter(int ipv6);
        
        void setLeastRestrictiveFilter();

    protected:
        bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif
