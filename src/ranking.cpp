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
#include <QFileInfo>

RsyncProcess::RsyncProcess (QObject *parent): QObject(parent)
{
    connect(&rsync, &QProcess::started, this, &RsyncProcess::startTimer);
    connect(&rsync, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RsyncProcess::getSpeed);
}

void RsyncProcess::init(int i, QString oneUrl, int theTimeout)
{
    index = i;
    url = oneUrl;
    timeout = theTimeout;
}

void RsyncProcess::start()
{
    path = QString("/tmp/core.db.%1").arg(index);

    QStringList arguments = { "-avL", "--no-h", "--no-motd", QString("--contimeout=%1").arg(timeout/1000), url, path };

    QString program = "rsync";

    rsync.start(program, arguments);
}

void RsyncProcess::startTimer()
{
    timer.start();
}

void RsyncProcess::getSpeed(int exitCode, QProcess::ExitStatus exitStatus)
{
    int timeElapsed = timer.elapsed();
    
    QFile file(path);
    qint64 fileSize = file.size();
    file.remove();

    double speed = 1000.0*fileSize/(1024.0*timeElapsed);

    emit processFinished(index, url, speed); 
}

RankingPerformer::RankingPerformer(QObject *parent) : QObject(parent)
{
    dbSubPath = "core/os/x86_64/core.db";
    connectionTimeout = 10000; // 10 seconds

    connect(&manager, &QNetworkAccessManager::finished, this, &RankingPerformer::requestFinished);
}

QStringList RankingPerformer::getByProtocol(QString protocol, QStringList urls)
{
    QStringList filteredUrls;

    for (int i = 0; i < urls.size(); i++) {
        if (urls.at(i).startsWith(protocol)) {
            filteredUrls.append(urls.at(i));
        }
    }

    return filteredUrls;
}

void RankingPerformer::rank(QStringList mirrorUrls)
{
    nRequests = mirrorUrls.size();
    nFinishedRequests = 0;

    QStringList httpUrls = getByProtocol("http", mirrorUrls);

    timers.clear();
    kibps.clear();

    for (int i = 0; i < httpUrls.size(); i++) {
        timers[httpUrls.at(i)] = QTime();
        timers[httpUrls.at(i)].start();

        QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(httpUrls[i].append(dbSubPath))));
        
        ReplyTimeout::set(reply, connectionTimeout);
    }

    if (QFileInfo::exists("/usr/bin/rsync")) {
        QStringList rsyncUrls = getByProtocol("rsync", mirrorUrls);
        
        rsyncProcesses.clear();

        for (int i = 0; i < rsyncUrls.size(); i++) {
            QString url = rsyncUrls.at(i);
            url.append(dbSubPath);

            rsyncProcesses.append(new RsyncProcess(this));
        
            rsyncProcesses.at(i)->init(i, url, connectionTimeout);

            connect(rsyncProcesses.at(i), &RsyncProcess::processFinished, this, &RankingPerformer::getSpeed);

            rsyncProcesses.at(i)->start();
        }
    }
}

void RankingPerformer::requestFinished(QNetworkReply *reply)
{
    QString url = reply->url().toString().replace(dbSubPath, QString(""));

    int timeElapsed = timers[url].elapsed();
    
    QByteArray data = reply->readAll();

    kibps[url] = 1000.0*data.size()/(1024.0*timeElapsed);

    nFinishedRequests++;

    if (nFinishedRequests == nRequests) {
        emit finished(kibps);
    }
    
    reply->deleteLater();
}

void RankingPerformer::getSpeed(int index, QString url, double speed)
{
    QString baseUrl = url.replace(dbSubPath, QString(""));

    kibps[baseUrl] = speed;

    nFinishedRequests++;

    if (nFinishedRequests == nRequests) {
        emit finished(kibps);
    }

    disconnect(rsyncProcesses.at(index), &RsyncProcess::processFinished, this, &RankingPerformer::getSpeed);
}
