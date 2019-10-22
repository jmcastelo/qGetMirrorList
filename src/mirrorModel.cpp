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

MirrorModel::MirrorModel(QObject *parent) : QAbstractTableModel(parent)
{
    // Connections
    connect(&theMirrorManager, &MirrorManager::mirrorListReady, this, &MirrorModel::setMirrorList);
    connect(&ranker, &RankingPerformer::finished, this, &MirrorModel::setMirrorSpeeds);
    connect(&updatemirrorlist, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MirrorModel::updateMirrorListFinished);
    connect(&updatemirrorlist, &QProcess::errorOccurred, this, &MirrorModel::updateMirrorListError);
}

void MirrorModel::getMirrorList()
{
    theMirrorManager.getMirrorList();
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
    Q_UNUSED(parent)
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
        if (col == 1) {
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

void MirrorModel::selectAllMirrors(bool selected)
{
    for (int i = 0; i< mirrorList.size(); i++) {
        mirrorList[i].selected = selected;
    }
}

void MirrorModel::selectMirror(QString url)
{
    for (int i = 0; i < mirrorList.size(); i++) {
        if (mirrorList.at(i).url == url) {
            mirrorList[i].selected = true;
            break;
        }
    }
}

void MirrorModel::deselectMirror(QString url)
{
    for (int i = 0; i < mirrorList.size(); i++) {
        if (mirrorList.at(i).url == url) {
            mirrorList[i].selected = false;
            break;
        }
    }
}

void MirrorModel::saveMirrorList(const QString file, bool allowRsync, QStringList urls)
{
    QFile outFile(file);

    if (outFile.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&outFile);
        
        QDateTime now = QDateTime::currentDateTime();
        QString nowString = now.toString("yyyy-MM-dd HH:mm:ss");
        
        out << "##" << "\n" << "## Arch Linux repository mirrorlist" << "\n";
        out << "## Generated on " << nowString << "\n";
        out << "## with qGetMirrorList" << "\n" << "##" << "\n";

        for (int i = 0; i < urls.size(); i++) {
            if ((urls.at(i).startsWith("rsync") && allowRsync) ||
                urls.at(i).startsWith("http")) {
                    out << "Server = " << urls.at(i) << "$repo/os/$arch\n";
            }
        }
    }
}

int MirrorModel::countSelectedMirrors()
{
    int nSelected = 0;

    for (int i = 0; i < mirrorList.size(); i++) {
        if (mirrorList.at(i).selected) {
            nSelected++;
        }
    }

    return nSelected;
}

void MirrorModel::rankMirrorList()
{
    QStringList mirrorUrls;

    for (int i = 0; i < mirrorList.size(); i++) {
        if (mirrorList.at(i).selected) {
            mirrorUrls.append(mirrorList.at(i).url);
        }
    }

    emit rankingMirrorsStarted();

    ranker.rank(mirrorUrls);
}

void MirrorModel::setMirrorSpeeds(QMap<QString, double> speeds)
{
    beginResetModel();
    for (int i = 0; i < mirrorList.size(); i++) {
        mirrorList[i].speed = speeds.value(mirrorList.at(i).url, mirrorList.at(i).speed);
        mirrorList[i].selected = false;
    }
    endResetModel();

    emit rankingMirrorsFinished(0);
}

bool MirrorModel::httpMirrorSelected()
{
    for (int i = 0; i < mirrorList.size(); i++) {
        if (mirrorList.at(i).selected && mirrorList.at(i).url.startsWith("http")) {
            return true;
        }
    }

    return false;
}

// Updating mirror list requires root privileges. Using 'pkexec' to elevate user to root.
void MirrorModel::updateMirrorList(QStringList urls)
{
    if (httpMirrorSelected()) {
        saveMirrorList("/tmp/mirrorlist", false, urls);

        QStringList args = { "cp", "/tmp/mirrorlist", "/etc/pacman.d/mirrorlist" };
        QString command = "pkexec";
    
        updatemirrorlist.start(command, args);
    } else {
        emit noHttpMirrorSelected();
    }
}
