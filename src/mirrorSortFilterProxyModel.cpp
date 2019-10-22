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

MirrorSortFilterProxyModel::MirrorSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    // Set least restrictive filter
    filter.protocolList.append("http");
    filter.protocolList.append("https");
    filter.protocolList.append("rsync");
    filter.active = 1;
    filter.isos = 1;
    filter.ipv4 = 1;
    filter.ipv6 = 1;
}

QVariant MirrorSortFilterProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Vertical) {
        return QStringLiteral("%1").arg(section + 1);
    }

    return sourceModel()->headerData(section, orientation, role);
}

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
    } else if (leftData.type() == QVariant::Time) {
        return leftData.toTime() < rightData.toTime();
    }

    return false;
}

bool MirrorSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
    QModelIndex index2 = sourceModel()->index(sourceRow, 2, sourceParent);
    QModelIndex index3 = sourceModel()->index(sourceRow, 8, sourceParent);
    QModelIndex index4 = sourceModel()->index(sourceRow, 9, sourceParent);
    QModelIndex index5 = sourceModel()->index(sourceRow, 10, sourceParent);
    QModelIndex index6 = sourceModel()->index(sourceRow, 11, sourceParent);

    QString country = sourceModel()->data(index1).toString();
    QString protocol = sourceModel()->data(index2).toString();
    QString ipv4 = sourceModel()->data(index3).toString();
    QString ipv6 = sourceModel()->data(index4).toString();
    QString active = sourceModel()->data(index5).toString();
    QString isos = sourceModel()->data(index6).toString();

    if ((filter.countryList.isEmpty() ||
        filter.countryList.contains(country)) &&
        filter.protocolList.contains(protocol) &&
        (filter.active == 1 ||
        (filter.active == 2 && active == "Yes") ||
        (filter.active == 0 && active == "No")) &&
        (filter.isos == 1 ||
        (filter.isos == 2 && isos == "Yes") ||
        (filter.isos == 0 && isos == "No")) &&
        (filter.ipv4 == 1 ||
        (filter.ipv4 == 2 && ipv4 == "Yes") ||
        (filter.ipv4 == 0 && ipv4 == "No")) &&
        (filter.ipv6 == 1 ||
        (filter.ipv6 == 2 && ipv6 == "Yes") ||
        (filter.ipv6 == 0 && ipv6 == "No"))) {
            return true;
    }

    return false;
}

void MirrorSortFilterProxyModel::appendCountryFilter(QString country)
{
    filter.countryList.append(country);
    invalidateFilter();
}

void MirrorSortFilterProxyModel::removeCountryFilter(QString country)
{
    filter.countryList.removeOne(country);
    invalidateFilter();
}

void MirrorSortFilterProxyModel::appendProtocolFilter(QString protocol)
{
    filter.protocolList.append(protocol);
    invalidateFilter();
}

void MirrorSortFilterProxyModel::removeProtocolFilter(QString protocol)
{
    filter.protocolList.removeOne(protocol);
    invalidateFilter();
}

void MirrorSortFilterProxyModel::setActiveFilter(int active)
{
    filter.active = active;
    invalidateFilter();
}

void MirrorSortFilterProxyModel::setIsosFilter(int isos)
{
    filter.isos = isos;
    invalidateFilter();
}

void MirrorSortFilterProxyModel::setIPv4Filter(int ipv4)
{
    filter.ipv4 = ipv4;
    invalidateFilter();
}

void MirrorSortFilterProxyModel::setIPv6Filter(int ipv6)
{
    filter.ipv6 = ipv6;
    invalidateFilter();
}

void MirrorSortFilterProxyModel::setLeastRestrictiveFilter()
{
    filter.countryList.clear();
    filter.protocolList.clear();
    filter.protocolList.append("http");
    filter.protocolList.append("https");
    filter.protocolList.append("rsync");
    filter.active = 1;
    filter.isos = 1;
    filter.ipv4 = 1;
    filter.ipv6 = 1;

    invalidateFilter();
}
