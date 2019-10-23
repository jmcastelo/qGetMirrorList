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
    connect(&rsync, &QProcess::errorOccurred, this, &RsyncProcess::processError);
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
    if (exitStatus == QProcess::NormalExit) {
        if (exitCode == 0) {
            int timeElapsed = timer.elapsed();

            QFile file(path);
            qint64 fileSize = file.size();
            file.remove();

            double speed = 1000.0*fileSize/(1024.0*timeElapsed);

            emit processFinished(index, url, speed);
        } else {
            
            emit processFailed(index, url, getRsyncError(exitCode));
        }
    } else {
        emit processFailed(index, url, QString("rsync crashed"));
    }
}

QString RsyncProcess::getRsyncError(int exitCode)
{
    QString errorMessage;

    switch (exitCode) {
        case 1:
            errorMessage = "Syntax or usage error";
            break;
        case 2:
            errorMessage = "Protocol incompatibility";
            break;
        case 3:
            errorMessage = "Errors selecting input/output files, dirs";
            break;
        case 4:
            errorMessage = "Requested action not supported";
            break;
        case 5:
            errorMessage = "Error starting client-server protocol";
            break;
        case 6:
            errorMessage = "Daemon unable to append to log-file";
            break;
        case 10:
            errorMessage = "Error in socket I/O";
            break;
        case 11:
            errorMessage = "Error in file I/O";
            break;
        case 12:
            errorMessage = "Error in rsync protocol data stream";
            break;
        case 13:
            errorMessage = "Errors with program diagnostics";
            break;
        case 14:
            errorMessage = "Error in IPC code";
            break;
        case 20:
            errorMessage = "Received SIGUSR1 or SIGINT";
            break;
        case 21:
            errorMessage = "Some error returned by waitpid()";
            break;
        case 22:
            errorMessage = "Error allocating core memory buffers";
            break;
        case 23:
            errorMessage = "Partial transfer due to error";
            break;
        case 24:
            errorMessage = "Partial transfer due to vanished source files";
            break;
        case 25:
            errorMessage = "The --max-delete limit stopped deletions";
            break;
        case 30:
            errorMessage = "Timeout in data send/receive";
            break;
        case 35:
            errorMessage = "Timeout waiting for daemon connection";
            break;
        default:
            errorMessage = QString("Error code %1").arg(exitCode);
    }

    return errorMessage;
}

void RsyncProcess::processError(QProcess::ProcessError error)
{
    QString errorMessage;

    switch (error) {
        case QProcess::FailedToStart:
            errorMessage = "rsync failed to start";
            break;
        case QProcess::Crashed:
            errorMessage = "rsync crashed";
            break;
        case QProcess::Timedout:
            errorMessage = "rsync timed out";
            break;
        default:
            errorMessage = "rsync error";
    }

    emit processFailed(index, url, errorMessage);
}

RankingPerformer::RankingPerformer(QObject *parent) : QObject(parent)
{
    dbSubPath = "core/os/x86_64/core.db";
    connectionTimeout = 5000; // 5 seconds

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
    nFinishedRequests = 0;

    errorMessage.clear();

    QStringList httpUrls = getByProtocol("http", mirrorUrls);

    nRequests = httpUrls.size();

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
        
        nRequests += rsyncUrls.size();

        rsyncProcesses.clear();

        for (int i = 0; i < rsyncUrls.size(); i++) {
            QString url = rsyncUrls.at(i);
            url.append(dbSubPath);

            rsyncProcesses.append(new RsyncProcess(this));
        
            rsyncProcesses.at(i)->init(i, url, connectionTimeout);

            connect(rsyncProcesses.at(i), &RsyncProcess::processFinished, this, &RankingPerformer::getSpeed);
            connect(rsyncProcesses.at(i), &RsyncProcess::processFailed, this, &RankingPerformer::rankingFailed);
            rsyncProcesses.at(i)->start();
        }
    }

    emit started(nRequests);
}

void RankingPerformer::requestFinished(QNetworkReply *reply)
{
    QString url = reply->url().toString().replace(dbSubPath, QString(""));

    int timeElapsed = timers[url].elapsed();
    
    QByteArray data = reply->readAll();

    kibps[url] = 1000.0*data.size()/(1024.0*timeElapsed);

    nFinishedRequests++;

    emit oneMirrorRanked(nFinishedRequests);

    checkIfFinished();
    
    reply->deleteLater();
}

void RankingPerformer::getSpeed(int index, QString url, double speed)
{
    QString baseUrl = url.replace(dbSubPath, QString(""));

    kibps[baseUrl] = speed;

    nFinishedRequests++;

    emit oneMirrorRanked(nFinishedRequests);

    checkIfFinished();    

    disconnect(rsyncProcesses.at(index), &RsyncProcess::processFinished, this, &RankingPerformer::getSpeed);

    rsyncProcesses.removeAt(index);
}

void RankingPerformer::checkIfFinished()
{
    if (nFinishedRequests == nRequests) {
        emit finished(kibps);
        if (!errorMessage.isEmpty()) {
            emit errors(errorMessage);
        }
    }
}

void RankingPerformer::rankingFailed(int index, QString url, QString message)
{
    QString baseUrl = url.replace(dbSubPath, QString(""));
    
    QString msg = QString("%1\n%2\n\n").arg(baseUrl).arg(message);

    errorMessage.append(msg);

    getSpeed(index, baseUrl, 0.0);
}
