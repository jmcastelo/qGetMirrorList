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

class MirrorParser
{
    private:
        QString rawData;
        QList<mirror> mirrorList;
        QList<Country> countryList;
        void parseMirrorList();
        void parseCountries();
        QPixmap getFlag(QString country);

    public:
        void setMirrorList(QString data);
        QList<mirror> getMirrorList();
        QList<Country> getCountryList();
};

#endif
