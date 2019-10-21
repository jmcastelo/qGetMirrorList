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
#include <QBasicTimer>
#include <QTimerEvent>

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
