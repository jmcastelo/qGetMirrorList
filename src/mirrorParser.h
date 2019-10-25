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

#ifndef MIRRORPARSER_H
#define MIRRORPARSER_H

#include "mirrorStruct.h"
#include <QList>
#include <QString>
#include <QStringList>
#include <QJsonArray>
#include <QTime>

class MirrorParser
{
    private:
        QJsonArray jsonMirrorArray;

        QList<Mirror> mirrorList;
        QList<Country> countryList;
        QDateTime lastCheck;
        
        void parseMirrorList();
        void parseCountries();
        
        QDateTime getLocalDateTime(QString date);
        QTime getDelay(int delaySeconds);
        QPixmap getFlag(QString country);
        QString getCountry(QString country);

    public:
        void setRawData(QByteArray data);
        
        QList<Mirror> getMirrorList();
        QList<Country> getCountryList();
        QDateTime getLastCheck();
};

#endif
