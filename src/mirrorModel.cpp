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

MirrorModel::MirrorModel(QObject *parent) : QAbstractTableModel(parent)
{
    // Connections
    connect(&theMirrorManager, &MirrorManager::mirrorListReady, this, &MirrorModel::setMirrorList);
    connect(&ranker, &RankingPerformer::finished, this, &MirrorModel::setMirrorSpeeds);
    //connect(&rankmirrors, &QProcess::started, this, &MirrorModel::rankingMirrors);
    //connect(&rankmirrors, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MirrorModel::readRankedMirrorList);
    //connect(&rankmirrors, &QProcess::errorOccurred, this, &MirrorModel::rankingMirrorsError);
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
    return mirrorList.size();
}

int MirrorModel::columnCount(const QModelIndex &parent) const
{
    return 11; 
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
        } else if (col == 7) {
            if (mirrorList.at(row).ipv4) {
                return tick; 
            } else {
                return cross;
            }
        } else if (col == 8) {
            if (mirrorList.at(row).ipv6) {
                return tick;
            } else {
                return cross;
            }
        } else if (col == 9) {
            if (mirrorList.at(row).active) {
                return tick;
            } else {
                return cross;
            }
        } else if (col == 10) {
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
        if (col == 0) {
            return mirrorList.at(row).url;
        } else if (col == 1) {
            return mirrorList.at(row).country;
        } else if (col == 2) {
            return mirrorList.at(row).protocol;
        } else if (col == 3) {
            return QString("%1").arg(mirrorList.at(row).completion_pct, 0, 'f', 2).toDouble();
        } else if (col == 4) {
            return QString("%1").arg(mirrorList.at(row).score, 0, 'f', 2).toDouble();
        } else if (col == 5) {
            return QString("%1").arg(mirrorList.at(row).speed, 0, 'f', 2).toDouble();
        } else if (col == 6) {
            return mirrorList.at(row).last_sync;
        } else if (col == 7) {
            return mirrorList.at(row).ipv4 ? QString("Yes") : QString("No");
        } else if (col == 8) {
            return mirrorList.at(row).ipv6 ? QString("Yes") : QString("No");
        } else if (col == 9) {
            return mirrorList.at(row).active ? QString("Yes") : QString("No");
        } else if (col == 10) {
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
        if (section == 0) {
            return QString("URL");
        } else if (section == 1) {
            return QString("Country");
        } else if (section == 2) {
            return QString("Protocol");
        } else if (section == 3) {
            return QString("Completion %");
        } else if (section == 4) {
            return QString("Score");
        } else if (section == 5) {
            return QString("Speed (KiB/s)");
        } else if (section == 6) {
            return QString("Last sync");
        } else if (section == 7) {
            return QString("IPv4");
        } else if (section == 8) {
            return QString("IPv6");
        } else if (section == 9) {
            return QString("Active");
        } else if (section == 10) {
            return QString("ISOs");
        }
    } else if (orientation == Qt::Vertical) {
        return QStringLiteral("%1").arg(section + 1);
    }

    return QVariant();
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

void MirrorModel::saveMirrorList(const QString file)
{
    QFile outFile(file);

    if (outFile.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&outFile);
        
        QDateTime now = QDateTime::currentDateTime();
        QString nowString = now.toString("yyyy-MM-dd HH:mm:ss");
        
        out << "##" << "\n" << "## Arch Linux repository mirrorlist" << "\n";
        out << "## Generated on " << nowString << "\n";
        out << "## By qGetMirrorList" << "\n" << "##" << "\n";

        for (int i = 0; i < mirrorList.size(); i++) {
            if (mirrorList.at(i).selected) {
                out << "Server = " << mirrorList.at(i).url << "\n";
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

/*void MirrorModel::rankMirrorList()
{
    QString path = "/tmp/mirrorlist";
    
    saveMirrorList(path);

    int nSelected = countSelectedMirrors();

    QStringList arguments = { "-n", QString::number(nSelected), path };
    QString program = "rankmirrors";

    rankmirrors.start(program, arguments);
}

// Parse output of 'rankmirrors' process to get ordered mirrors
void MirrorModel::readRankedMirrorList(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode == 0) {
        QString output = QString(rankmirrors.readAllStandardOutput());

        output.replace(QString("Server = "), QString(""));

        QStringList urls = output.split("\n");

        QList<Mirror> rankedMirrorList;

        for (int i = 0; i < urls.size(); i++) {
            for (int j = 0; j < mirrorList.size(); j++) {
                if (urls.at(i) == mirrorList.at(j).url) {
                    mirrorList[j].selected = false;
                    rankedMirrorList.append(mirrorList.at(j));
                    break;
                }
            }
        }
        
        beginResetModel();
        mirrorList = rankedMirrorList;
        endResetModel();
    
        emit rankingMirrorsEnd(0);
        emit mirrorListSet();
    }
}

void MirrorModel::cancelRankingMirrorList()
{
    rankmirrors.close();
    emit rankingMirrorsCancelled(0);
}*/

// Updating mirror list requires root privileges. Using 'pkexec' to elevate user to root.
void MirrorModel::updateMirrorList()
{
    saveMirrorList("/tmp/mirrorlist");

    QStringList args = { "cp", "/tmp/mirrorlist", "/etc/pacman.d/mirrorlist" };
    QString command = "pkexec";
    
    updatemirrorlist.start(command, args);
}
