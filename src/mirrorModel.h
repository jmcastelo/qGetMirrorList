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

#include "mirrorStruct.h"
#include "ranking.h"
#include <QAbstractTableModel>
#include <QVariant>
#include <QString>
#include <QProcess>

class MirrorModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        MirrorModel(QObject *parent = nullptr);
        
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
        
        void selectAllMirrors(bool selected);
        void selectMirror(QString url, bool selected);
        void saveMirrorList(const QString file, bool allowRsync, QStringList urls);
        void rankMirrorList();
        void updateMirrorList(QStringList urls);

    signals:
        void mirrorListSet();
        void rankingMirrorsStarted();
        void rankingMirrorsFinished(int r);
        void rankingMirrorsErrors(QString errorMessage);
        void setProgressBarMax(int max);
        void setProgressBarValue(int value);
        void updateMirrorListFinished(int exitCode, QProcess::ExitStatus exitStatus);
        void updateMirrorListError(QProcess::ProcessError error);
        void noHttpMirrorSelected();

    public slots:
        void setMirrorList(QList<Mirror> ml);

    private slots:
        void setMirrorSpeeds(QMap<QString, double> speeds);

    private:
        QList<Mirror> mirrorList;
        QProcess updatemirrorlist;
        RankingPerformer ranker;
        bool httpMirrorSelected();
};

#endif
