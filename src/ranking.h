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
        void init(int i, QString oneUrl, int theTimeout);
        void start();

    signals:
        void processFinished(int index, QString url, double speed);
        void processFailed(int index, QString url, QString message);

    private slots:
        void startTimer();
        void getSpeed(int exitCode, QProcess::ExitStatus exitStatus);
        void processError(QProcess::ProcessError error);

    private:
        int index;
        int timeout;
        QString url;
        QString path;
        QTime timer;
        double speed;
        QProcess rsync;
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

    private slots:
        void requestFinished(QNetworkReply *reply);
        void getSpeed(int index, QString url, double speed);
        void rankingFailed(int index, QString url, QString message);

    private:
        QNetworkAccessManager manager;
        
        QString dbSubPath;
        int httpConnectionTimeout;
        int rsyncConnectionTimeout;
        
        int nRequests;
        int nFinishedRequests;
        
        QList<RsyncProcess*> rsyncProcesses;
        
        QMap<QString, QTime> timers;
        QMap<QString, double> kibps;

        QStringList getUrlsByProtocol(QString protocol, QStringList urls);
        QString getReplyErrorMessage(QNetworkReply::NetworkError error);

        void checkIfFinished();

        QString errorMessage;
};

class ReplyTimeout : public QObject
{
    Q_OBJECT

    public:
        enum HandleMethod { Abort, Close };
        
        ReplyTimeout(QNetworkReply* reply, const int timeout, HandleMethod method = Abort) : QObject(reply), m_method(method)
        {
            Q_ASSERT(reply);
            
            if (reply && reply->isRunning()) {
                m_timer.start(timeout, this);
            }
        }
        
        static void set(QNetworkReply* reply, const int timeout, HandleMethod method = Abort)
        {
            new ReplyTimeout(reply, timeout, method);
        }

    protected:
        void timerEvent(QTimerEvent *ev)
        {
            if (!m_timer.isActive() || ev->timerId() != m_timer.timerId()) {
                return;
            }
    
            auto reply = static_cast<QNetworkReply*>(parent());
    
            if (reply->isRunning()) {
                if (m_method == Close) {
                    reply->close();
                } else if (m_method == Abort) {
                    reply->abort();
                }
            }
            
            m_timer.stop();
        }
    
    private:
        QBasicTimer m_timer;
        HandleMethod m_method;
};

#endif
