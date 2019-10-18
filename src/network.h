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

#ifndef NETWORK_H
#define NETWORK_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QObject>

class Network : public QObject
{
    Q_OBJECT

    public:
        explicit Network(QObject *parent = 0);
        virtual ~Network();
        void getRequest(QUrl url);
        QString getData() const;

    signals:
        void dataRead(QUrl url);

    private slots:
        void readData(QNetworkReply *reply);

    private:
        QNetworkAccessManager manager;
        QString data;
};

#endif
