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

#ifndef MIRRORMANAGER_H
#define MIRRORMANAGER_H

#include "network.h"
#include "mirrorParser.h"
#include <QObject>
#include <QList>
#include <QUrl>

struct MirrorFilter
{
    QStringList countryList;
    QStringList protocolList;
    int ipv4;
    int ipv6;
    bool statusOK;
    bool statusKO;
};

class MirrorManager : public QObject
{
    Q_OBJECT

    public:
        MirrorManager();
        
        void getMirrorList();
        void getCountryList();

        QList<Mirror> filterMirrorList(MirrorFilter filter);

    signals:
        void mirrorListReady(QList<Mirror> mirrorList);
        void countryListReady(QList<Country> countryList);

    private slots:
        void getParsedData();

    private:
        Network theNetwork;
        MirrorParser theParser;
        QList<Mirror> mirrorListAll;
        QUrl url;
        bool gettingMirrorList;
        bool gettingCountryList;
};

#endif
