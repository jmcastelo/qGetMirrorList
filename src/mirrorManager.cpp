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

#include "mirrorManager.h"

MirrorManager::MirrorManager()
{
    // Url to query for mirrors (JSON format) 
    url = "https://www.archlinux.org/mirrors/status/json/";

    connect(&theNetwork, &Network::dataRead, this, &MirrorManager::getParsedData);
}

// Parse fetched mirror list according to Url
void MirrorManager::getParsedData()
{
    theParser.setRawData(theNetwork.getData());

    if (gettingMirrorList) {
        mirrorListAll = theParser.getMirrorList();
        emit mirrorListReady(mirrorListAll);
        gettingMirrorList = false;
    }

    if (gettingCountryList) {
        emit countryListReady(theParser.getCountryList());
        gettingCountryList = false;
    }
}

void MirrorManager::getMirrorList()
{
    gettingMirrorList = true;
    theNetwork.getRequest(url);
}

void MirrorManager::getCountryList()
{
    gettingCountryList = true;
    theNetwork.getRequest(url);
}

QList<Mirror> MirrorManager::filterMirrorList(MirrorFilter filter)
{
    // Don't filter if least restrictive filter set, just return all mirrors
    if (filter.countryList.isEmpty() &&
        filter.protocolList.contains("http") &&
        filter.protocolList.contains("https") &&
        filter.protocolList.contains("rsync") &&
        filter.active == 1 &&
        filter.isos == 1 &&
        filter.ipv4 == 1 &&
        filter.ipv6 == 1) {
            return mirrorListAll;
    }

    QList<Mirror> filteredMirrorList;

    // Do the filtering
    for (int i = 0; i < mirrorListAll.size(); i++) {
        if ((filter.countryList.isEmpty() ||
            filter.countryList.contains(mirrorListAll.at(i).country)) &&
            filter.protocolList.contains(mirrorListAll.at(i).protocol) &&
            (filter.active== 1 ||
            (filter.active == 2 && mirrorListAll.at(i).active) ||
            (filter.active == 0 && !mirrorListAll.at(i).active)) &&
            (filter.isos== 1 ||
            (filter.isos== 2 && mirrorListAll.at(i).isos) ||
            (filter.isos== 0 && !mirrorListAll.at(i).isos)) &&
            (filter.ipv4 == 1 ||
            (filter.ipv4 == 2 && mirrorListAll.at(i).ipv4) ||
            (filter.ipv4 == 0 && !mirrorListAll.at(i).ipv4)) &&
            (filter.ipv6 == 1 ||
            (filter.ipv6 == 2 && mirrorListAll.at(i).ipv6) ||
            (filter.ipv6 == 0 && !mirrorListAll.at(i).ipv6))) { 
                filteredMirrorList.append(mirrorListAll.at(i));
        }
    }
    
    return filteredMirrorList;
}
