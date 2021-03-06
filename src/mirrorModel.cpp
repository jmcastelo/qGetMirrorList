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

#include "mirrorModel.h"
#include "columns.h"
#include <algorithm>

MirrorModel::MirrorModel(QObject *parent) : QAbstractTableModel(parent)
{
    // Connections
    connect(&ranker, &RankingPerformer::started, this, &MirrorModel::setProgressBarMax);
    connect(&ranker, &RankingPerformer::oneMirrorRanked, this, &MirrorModel::setProgressBarValue);
    connect(&ranker, &RankingPerformer::finished, this, &MirrorModel::setMirrorSpeeds);
    connect(&ranker, &RankingPerformer::errors, this, &MirrorModel::rankingMirrorsErrors);
    connect(&updatemirrorlist, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MirrorModel::updateMirrorListFinished);
    connect(&updatemirrorlist, &QProcess::errorOccurred, this, &MirrorModel::updateMirrorListError);
}

void MirrorModel::setMirrorList(QList<Mirror> ml)
{
    beginResetModel();
    mirrorList = ml;
    endResetModel();

    emit mirrorListSet();
}

int MirrorModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return mirrorList.size();
}

int MirrorModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 12; 
}

QVariant MirrorModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= mirrorList.size()) {
        return QVariant();
    }

    int row = index.row();
    int col = index.column();

    QPixmap tick(":/images/icons/tick.png");
    QPixmap cross(":/images/icons/cross.png");

    if (role == Qt::DecorationRole) {
        if (col == Columns::country) {
            return mirrorList.at(row).flag;
        } else if (col == Columns::ipv4) {
            if (mirrorList.at(row).ipv4) {
                return tick; 
            } else {
                return cross;
            }
        } else if (col == Columns::ipv6) {
            if (mirrorList.at(row).ipv6) {
                return tick;
            } else {
                return cross;
            }
        } else if (col == Columns::active) {
            if (mirrorList.at(row).active) {
                return tick;
            } else {
                return cross;
            }
        } else if (col == Columns::isos) {
            if (mirrorList.at(row).isos) {
                return tick;
            } else {
                return cross;
            }
        } else {
            return QVariant();
        }
    }

    if (role == Qt::DisplayRole) {
        if (col == Columns::url) {
            return mirrorList.at(row).url;
        } else if (col == Columns::country) {
            return mirrorList.at(row).country;
        } else if (col == Columns::protocol) {
            return mirrorList.at(row).protocol;
        } else if (col == Columns::completion_pct) {
            return QString("%1").arg(mirrorList.at(row).completion_pct, 0, 'f', 2).toDouble();
        } else if (col == Columns::score) {
            return QString("%1").arg(mirrorList.at(row).score, 0, 'f', 2).toDouble();
        } else if (col == Columns::speed) {
            return QString("%1").arg(mirrorList.at(row).speed, 0, 'f', 2).toDouble();
        } else if (col == Columns::last_sync) {
            return mirrorList.at(row).last_sync;
        } else if (col == Columns::delay) {
            return mirrorList.at(row).delay;
        } else if (col == Columns::ipv4) {
            return mirrorList.at(row).ipv4 ? QString("Yes") : QString("No");
        } else if (col == Columns::ipv6) {
            return mirrorList.at(row).ipv6 ? QString("Yes") : QString("No");
        } else if (col == Columns::active) {
            return mirrorList.at(row).active ? QString("Yes") : QString("No");
        } else if (col == Columns::isos) {
            return mirrorList.at(row).isos ? QString("Yes") : QString("No");
        } else {
            return QVariant();
        }
    }

    return QVariant();
}

QVariant MirrorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        if (section == Columns::url) {
            return QString("URL");
        } else if (section == Columns::country) {
            return QString("Country");
        } else if (section == Columns::protocol) {
            return QString("Protocol");
        } else if (section == Columns::completion_pct) {
            return QString("%");
        } else if (section == Columns::score) {
            return QString("Score");
        } else if (section == Columns::speed) {
            return QString("Speed");
        } else if (section == Columns::last_sync) {
            return QString("Last sync");
        } else if (section == Columns::delay) {
            return QString("Delay");
        } else if (section == Columns::ipv4) {
            return QString("IPv4");
        } else if (section == Columns::ipv6) {
            return QString("IPv6");
        } else if (section == Columns::active) {
            return QString("Active");
        } else if (section == Columns::isos) {
            return QString("ISOs");
        }
    } else if (orientation == Qt::Vertical) {
        return QStringLiteral("%1").arg(section + 1);
    }

    return QVariant();
}

bool rowComparator(QModelIndex index1, QModelIndex index2)
{
    return index1.row() < index2.row();
}

void MirrorModel::sortModelIndexListByRow(QModelIndexList *list)
{
    std::sort(list->begin(), list->end(), rowComparator);
}

void MirrorModel::saveMirrorList(const QString file, QModelIndexList urlIndexes)
{
    QFile outFile(file);

    if (outFile.open(QFile::WriteOnly | QFile::Truncate)) {
        sortModelIndexListByRow(&urlIndexes);

        QTextStream out(&outFile);
        
        QDateTime now = QDateTime::currentDateTime();
        QString nowString = now.toString("yyyy-MM-dd HH:mm:ss");
        
        out << "##" << "\n" << "## Arch Linux repository mirrorlist" << "\n";
        out << "## Generated on " << nowString << "\n";
        out << "## with qGetMirrorList" << "\n" << "##" << "\n";

        for (int i = 0; i < urlIndexes.size(); i++) {
            out << "Server = " << urlIndexes.at(i).data().toString() << "$repo/os/$arch\n";
        }
    }
}

void MirrorModel::rankMirrorList(QModelIndexList urlIndexes)
{
    QStringList urls;

    for (int i = 0; i < urlIndexes.size(); i++) {
        urls.append(urlIndexes.at(i).data().toString());
    }

    emit rankingMirrorsStarted();

    ranker.rank(urls);
}

void MirrorModel::setMirrorSpeeds(QMap<QString, double> speeds)
{
    // Map: <URL, speed>
    QMap<QString, double>::const_iterator map;
    for (map = speeds.constBegin(); map != speeds.constEnd(); ++map) {
        // Search index that matches url (map.key()) starting from row=0, col=0 (url column)
        QModelIndexList indexes = match(index(0, 0), Qt::DisplayRole, map.key(), 1);
        // Update mirror speed (map.value())
        int row = indexes.first().row();
        mirrorList[row].speed = map.value();
        // Matched index is url column, get speed column from it to update view
        QModelIndex speedIndex = indexes.first().siblingAtColumn(Columns::speed);
        emit dataChanged(speedIndex, speedIndex);
    }

    emit rankingMirrorsFinished(0);
}

void MirrorModel::cancelRankMirrorList()
{
    ranker.cancelRanking();
}

// Updating mirror list requires root privileges. Using 'pkexec' to elevate user to root.
void MirrorModel::updateMirrorList(QModelIndexList urlIndexes)
{
    // Filter only http urls
    filterHttpIndexes(&urlIndexes);

    if (urlIndexes.isEmpty()) {
        emit noHttpMirrorSelected();
    } else {
        // Backup existing mirrorlist
        QProcess backup;
        backup.start("cp", QStringList() << "/etc/pacman.d/mirrorlist" << "/tmp/mirrorlist.backup");
        backup.waitForFinished();

        // Update mirrorlist
        saveMirrorList("/tmp/mirrorlist", urlIndexes);

        QStringList args = { "cp", "/tmp/mirrorlist", "/etc/pacman.d/mirrorlist" };
        QString command = "pkexec";

        updatemirrorlist.start(command, args);
    }
}

void MirrorModel::filterHttpIndexes(QModelIndexList *urlIndexes)
{
    QList<QModelIndex>::iterator index;
    for (index = urlIndexes->begin(); index != urlIndexes->end();) {
        if (index->siblingAtColumn(Columns::protocol).data().toString() == "rsync") {
            index = urlIndexes->erase(index);
        } else {
            ++index;
        }
    }
}
