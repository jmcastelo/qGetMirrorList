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

    connect(&manager, &QNetworkAccessManager::finished, this, &DataSource::requestFinished);
}

void DataSource::getSourceData()
{
    manager.get(QNetworkRequest(sourceUrl));
}

void DataSource::requestFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        getParsedData(reply->readAll());
    } else {
        QString message = getReplyErrorMessage(reply->error());
        QString errorMessage = QString("Error getting JSON data from\nhttps://www.archlinux.org/mirrors/status/json/\n\n%1").arg(message);
        emit networkReplyError(errorMessage);
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

QString DataSource::getReplyErrorMessage(QNetworkReply::NetworkError error)
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
        case QNetworkReply::OperationCanceledError:
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
