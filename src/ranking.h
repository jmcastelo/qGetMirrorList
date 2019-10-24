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

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QBasicTimer>
#include <QTimerEvent>
#include <QProcess>
#include <QFile>

class RsyncProcess : public QObject
{
    Q_OBJECT

    public:
        RsyncProcess(QObject *parent = nullptr);
        QProcess rsync;
        void init(int i, QString oneUrl, int theTimeout);
        void start();

    signals:
        void processFinished(int index, QString url, double speed);
        void processFailed(int index, QString url, QString message);
        void processCancelled(int index);

    private slots:
        void startTimer();
        void getSpeed(int exitCode, QProcess::ExitStatus exitStatus);

    private:
        int index;
        int timeout;
        QString url;
        QString path;
        QTime timer;
        double speed;
        QString getRsyncError(int exitCode);
};

class RankingPerformer : public QObject
{
    Q_OBJECT

    public:
        RankingPerformer(QObject *parent = nullptr);
        void rank(QStringList mirrorUrls);

    signals:
        void started(int max);
        void oneMirrorRanked(int value);
        void finished(QMap<QString, double> speeds);
        void errors(QString errorMessage);
        void cancelRanking();

    private slots:
        void requestFinished(QNetworkReply *reply);
        void rsyncFinished(int index, QString url, double speed);
        void rsyncFailed(int index, QString url, QString message);
        void rsyncCancelled(int index);

    private:
        QNetworkAccessManager manager;
        
        QString dbSubPath;
        int rsyncConnectionTimeout;
        
        int nRequests;
        int nFinishedRequests;
        
        QList<RsyncProcess*> rsyncProcesses;

        QMap<QString, QNetworkReply*> replies;
        
        QMap<QString, QTime> timers;
        QMap<QString, double> kibps;

        QStringList getUrlsByProtocol(QString protocol, QStringList urls);
        QString getReplyErrorMessage(QNetworkReply::NetworkError error);

        void checkIfFinished();

        QString errorMessage;
};

#endif
