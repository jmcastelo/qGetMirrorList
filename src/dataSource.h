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

#ifndef DATASOURCE_H
#define DATASOURCE_H

#include "mirrorParser.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>


class DataSource : public QObject
{
    Q_OBJECT

    public:
        DataSource(QObject *parent = nullptr);
        void getSourceData();

    signals:
        void mirrorListReady(QList<Mirror> mirrorList);
        void countryListReady(QList<Country> countryList);
        void networkReplyError(QString errorMesssage);

    private slots:
        void requestFinished(QNetworkReply *reply);

    private:
        QNetworkAccessManager manager;
        QString sourceUrl;
        MirrorParser parser;
        QList<Mirror> mirrorList;
        QList<Country> countryList;

        void getParsedData(QByteArray sourceData);
        QString getReplyErrorMessage(QNetworkReply::NetworkError error);
};

#endif
