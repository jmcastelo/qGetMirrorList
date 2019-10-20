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

#ifndef RANKING_H
#define RANKING_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

struct Unit
{
    QUrl ulr;
    double kibps;
};

class RankingPerformer : public QObject
{
    Q_OBJECT

    public:
        RankingPerformer(QObject *parent = 0);
        void rank(QStringList mirrorUrls);

    signals:
        void finished(QMap<QString, double> speeds);

    private slots:
        void requestFinished(QNetworkReply *reply);

    private:
        QNetworkAccessManager manager;
        QString dbSubPath;
        int nRequests;
        int nFinishedRequests;
        QMap<QString, QTime> timers;
        QMap<QString, double> kibps;
};

#endif
