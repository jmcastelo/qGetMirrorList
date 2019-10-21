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

#include "ranking.h"

RankingPerformer::RankingPerformer(QObject *parent) : QObject(parent)
{
    dbSubPath = "core/os/x86_64/core.db";

    connect(&manager, &QNetworkAccessManager::finished, this, &RankingPerformer::requestFinished);
}

void RankingPerformer::rank(QStringList mirrorUrls)
{
    nRequests = mirrorUrls.size();
    nFinishedRequests = 0;

    timers.clear();
    kibps.clear();

    for (int i = 0; i < mirrorUrls.size(); i++) {
        timers[mirrorUrls.at(i)] = QTime();
        timers[mirrorUrls.at(i)].start();
        QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(mirrorUrls[i].append(dbSubPath))));
        ReplyTimeout::set(reply, 10000);
    }

}

void RankingPerformer::requestFinished(QNetworkReply *reply)
{
    QString url = reply->url().toString().replace(dbSubPath, QString(""));

    double timeElapsed = timers[url].elapsed();
    
    QByteArray data = reply->readAll();

    kibps[url] = 1000*data.size()/(1024*timeElapsed);

    nFinishedRequests++;

    if (nFinishedRequests == nRequests) {
        emit finished(kibps);
    }
    
    reply->deleteLater();
}
