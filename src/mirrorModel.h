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

#ifndef MIRRORMODEL_H
#define MIRRORMODEL_H

#include "mirrorManager.h"
#include "ranking.h"
#include <QAbstractTableModel>
#include <QVariant>
#include <QString>
#include <QProcess>

class MirrorModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        MirrorModel(QObject *parent = 0);
        
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
        
        void selectAllMirrors(bool selected);
        void selectMirror(QString url);
        void deselectMirror(QString url);
        void rankMirrorList();

    signals:
        void mirrorListSet();
        void rankingMirrorsStarted();
        void rankingMirrorsFinished(int r);
        void updateMirrorListFinished(int exitCode, QProcess::ExitStatus exitStatus);
        void updateMirrorListError(QProcess::ProcessError error);

    public slots:
        void getMirrorList();
        void saveMirrorList(const QString file);
        void updateMirrorList();

    private slots:
        void setMirrorList(QList<Mirror> ml);
        void setMirrorSpeeds(QMap<QString, double> speeds);

    private:
        QList<Mirror> mirrorList;
        QProcess updatemirrorlist;
        MirrorManager theMirrorManager;
        RankingPerformer ranker;
        int countSelectedMirrors();
};

#endif
