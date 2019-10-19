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

#include "mirrorParser.h"
#include <QJsonDocument>
#include <QJsonObject>

void MirrorParser::setRawData(QByteArray data)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = jsonDocument.object();
    jsonMirrorArray = jsonObject["urls"].toArray();
}

QList<Mirror> MirrorParser::getMirrorList()
{
    parseMirrorList();
    return mirrorList;
}

void MirrorParser::parseMirrorList()
{
    Mirror oneMirror;
    oneMirror.url = "";
    oneMirror.protocol = "";
    oneMirror.last_sync = "";
    oneMirror.completion_pct = -1;
    oneMirror.delay = -1;
    oneMirror.duration_avg = -1;
    oneMirror.duration_stddev = -1;
    oneMirror.score = -1;
    oneMirror.active = false;
    oneMirror.country = "";
    oneMirror.country_code = "";
    oneMirror.isos = false;
    oneMirror.ipv4 = false;
    oneMirror.ipv6 = false;
    oneMirror.status = true;
    oneMirror.selected = false;
    oneMirror.flag = QPixmap();

    mirrorList.clear();

    for (const QJsonValue &value : jsonMirrorArray) {
        QJsonObject loopObject = value.toObject();

        oneMirror.url = loopObject["url"].toString();
        oneMirror.protocol = loopObject["protocol"].toString();
        oneMirror.last_sync = loopObject["last_sync"].toString();
        oneMirror.completion_pct = loopObject["completion_pct"].toDouble();
        oneMirror.delay = loopObject["delay"].toInt();
        oneMirror.duration_avg = loopObject["duration_avg"].toDouble();
        oneMirror.duration_stddev = loopObject["duration_stddev"].toDouble();
        oneMirror.score = loopObject["score"].toDouble();
        oneMirror.active = loopObject["active"].toBool();
        oneMirror.country = getCountry(loopObject["country"].toString());
        oneMirror.country_code = loopObject["country_code"].toString().toLower();
        oneMirror.isos = loopObject["isos"].toBool();
        oneMirror.ipv4 = loopObject["ipv4"].toBool();
        oneMirror.ipv6 = loopObject["ipv6"].toBool();
        oneMirror.status = true;
        oneMirror.selected = false;
        oneMirror.flag = getFlag(oneMirror.country_code);

        mirrorList.append(oneMirror);
    }
}

QString MirrorParser::getCountry(QString country)
{
    if (country.isEmpty()) {
        return "Worldwide";
    }

    return country;
}

QPixmap MirrorParser::getFlag(QString country_code)
{
    if (country_code.isEmpty()) {
        country_code = "world";
    }

    QString resource = ":/images/flags/";
    resource += country_code;
    resource += ".png";

    return QPixmap(resource);
}

QList<Country> MirrorParser::getCountryList()
{
    parseCountries();
    return countryList;
}

void MirrorParser::parseCountries()
{
    QStringList uniqueCountryList;

    Country oneCountry;

    countryList.clear();

    for (const QJsonValue &value : jsonMirrorArray) {
        QJsonObject loopObject = value.toObject();
        
        oneCountry.name = getCountry(loopObject["country"].toString());
        oneCountry.code = loopObject["country_code"].toString().toLower();
        oneCountry.flag = getFlag(oneCountry.code);

        if (!uniqueCountryList.contains(oneCountry.name)) {
            uniqueCountryList.append(oneCountry.name);
            countryList.append(oneCountry);
        }
    }
}
