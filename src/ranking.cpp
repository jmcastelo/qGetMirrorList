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

RsyncProcess::RsyncProcess (QObject *parent, int theIndex, QString theUrl, int theTimeout): QObject(parent)
{
    index = theIndex;
    url = theUrl;
    timeout = theTimeout;

    connect(&rsync, &QProcess::started, this, &RsyncProcess::startTimer);
    connect(&rsync, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RsyncProcess::getSpeed);
}

void RsyncProcess::start()
{
    path = QString("/tmp/core.db.%1").arg(index);

    QStringList arguments = { "-avL", "--no-h", "--no-motd", QString("--contimeout=%1").arg(timeout), url, path };

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

            emit processFinished(url, speed);
        } else {
            emit processFailed(url, getRsyncError(exitCode));
        }
    } else {
        emit processCanceled();
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

RankingPerformer::RankingPerformer(QObject *parent) : QObject(parent)
{
    dbSubPath = "extra/os/x86_64/extra.db";
    //dbSubPath = "core/os/x86_64/core.db";
    rsyncConnectionTimeout = 5; // 5 seconds

    connect(&manager, &QNetworkAccessManager::finished, this, &RankingPerformer::requestFinished);
}

QStringList RankingPerformer::getUrlsByProtocol(QString protocol, QStringList urls)
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

    QStringList httpUrls = getUrlsByProtocol("http", mirrorUrls);

    nRequests = httpUrls.size();

    timers.clear();
    kibps.clear();
    replies.clear();

    for (int i = 0; i < httpUrls.size(); i++) {
        timers[httpUrls.at(i)] = QTime();
        timers[httpUrls.at(i)].start();

        QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(httpUrls[i].append(dbSubPath))));

        replies[httpUrls.at(i)] = reply;

        connect(this, &RankingPerformer::cancelRanking, replies[httpUrls.at(i)], &QNetworkReply::abort);
    }

    if (QFileInfo::exists("/usr/bin/rsync")) {
        QStringList rsyncUrls = getUrlsByProtocol("rsync", mirrorUrls);
        
        nRequests += rsyncUrls.size();

        rsyncProcesses.clear();

        for (int i = 0; i < rsyncUrls.size(); i++) {
            QString url = rsyncUrls.at(i);
            url.append(dbSubPath);

            rsyncProcesses.append(new RsyncProcess(this, i, url, rsyncConnectionTimeout));
        
            connect(rsyncProcesses.at(i), &RsyncProcess::processFinished, this, &RankingPerformer::rsyncFinished);
            connect(rsyncProcesses.at(i), &RsyncProcess::processFailed, this, &RankingPerformer::rsyncFailed);
            connect(rsyncProcesses.at(i), &RsyncProcess::processCanceled, this, &RankingPerformer::rsyncCanceled);
            connect(this, &RankingPerformer::cancelRanking, &rsyncProcesses.at(i)->rsync, &QProcess::kill);

            rsyncProcesses.at(i)->start();
        }
    }

    emit started(nRequests);
}

void RankingPerformer::requestFinished(QNetworkReply *reply)
{
    QString url = reply->url().toString().replace(dbSubPath, QString(""));

    if (reply->error() == QNetworkReply::NoError) {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (statusCode == 200) {
            int timeElapsed = timers[url].elapsed();

            QByteArray data = reply->readAll();

            kibps[url] = 1000.0*data.size()/(1024.0*timeElapsed);
        } else {
            QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

            errorMessage.append(QString("%1\n%2: %3\n\n").arg(url).arg(statusCode).arg(reason));
        }
    } else if (reply->error() != QNetworkReply::OperationCanceledError) {
        QString message = getReplyErrorMessage(reply->error());
        
        QString msg = QString("%1\n%2\n\n").arg(url).arg(message);

        errorMessage.append(msg);
    }

    nFinishedRequests++;

    emit oneMirrorRanked(nFinishedRequests);

    checkIfFinished();

    reply->deleteLater();
}

QString RankingPerformer::getReplyErrorMessage(QNetworkReply::NetworkError error)
{
    QString message;

    switch (error) {
        case QNetworkReply::ConnectionRefusedError:
            message = "Connection refused";
            break;
        case QNetworkReply::RemoteHostClosedError:
            message = "Remote host closed connection";
            break;
        case QNetworkReply::HostNotFoundError:
            message = "Host not found";
            break;
        case QNetworkReply::TimeoutError:
            message = "Connection timed out";
            break;
        case QNetworkReply::NetworkSessionFailedError:
            message = "Connection broken";
            break;
        case QNetworkReply::ContentAccessDenied:
            message = "Access to remote content denied (403)";
            break;
        case QNetworkReply::ContentNotFoundError:
            message = "Content not found (404)";
            break;
        case QNetworkReply::ServiceUnavailableError:
            message = "Service unavailable";
            break;
        case QNetworkReply::UnknownServerError:
            message = "Unknown server error";
            break;
        default:
            message = "Connection error";
    }
    
    return message;
}

void RankingPerformer::rsyncFinished(QString url, double speed)
{
    QString baseUrl = url.replace(dbSubPath, QString(""));

    kibps[baseUrl] = speed;

    nFinishedRequests++;

    emit oneMirrorRanked(nFinishedRequests);

    checkIfFinished();
}

void RankingPerformer::rsyncFailed(QString url, QString message)
{
    QString baseUrl = url.replace(dbSubPath, QString(""));
    
    QString msg = QString("%1\n%2\n\n").arg(baseUrl).arg(message);

    errorMessage.append(msg);

    nFinishedRequests++;

    emit oneMirrorRanked(nFinishedRequests);

    checkIfFinished();
}

void RankingPerformer::rsyncCanceled()
{
    nFinishedRequests++;

    emit oneMirrorRanked(nFinishedRequests);

    checkIfFinished();
}

void RankingPerformer::checkIfFinished()
{
    if (nFinishedRequests == nRequests) {

        emit finished(kibps);

        rsyncProcesses.clear();
        replies.clear();
        timers.clear();

        if (!errorMessage.isEmpty()) {
            emit errors(errorMessage);
        }
    }
}
