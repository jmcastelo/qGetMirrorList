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

#include "dataSource.h"

DataSource::DataSource(QObject *parent) : QObject(parent)
{
    sourceUrl = "https://www.archlinux.org/mirrors/status/json/";

    connect(&manager, &QNetworkAccessManager::finished, this, &DataSource::readData);
}

void DataSource::getSourceData()
{
    manager.get(QNetworkRequest(sourceUrl));
}

void DataSource::readData(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        getParsedData(reply->readAll());
    } else {
        emit networkReplyError(reply->error());
    }
    reply->deleteLater();
}

void DataSource::getParsedData(QByteArray sourceData)
{
    parser.setRawData(sourceData);

    mirrorList = parser.getMirrorList();
    countryList = parser.getCountryList();

    emit mirrorListReady(mirrorList);
    emit countryListReady(countryList);
}
