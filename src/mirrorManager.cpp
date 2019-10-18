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
    urls.clear();
    
    urls.append(QUrl("https://www.archlinux.org/mirrorlist/all/"));
    urls.append(QUrl("https://www.archlinux.org/mirrorlist/?country=all&ip_version=4"));
    urls.append(QUrl("https://www.archlinux.org/mirrorlist/?country=all&ip_version=6"));
    urls.append(QUrl("https://www.archlinux.org/mirrorlist/?country=all&use_mirror_status=on"));

    connect(&theNetwork, &Network::dataRead, this, &MirrorManager::parseMirrorList);
    connect(&theNetwork, &Network::dataRead, this, &MirrorManager::parseCountryList);
}

void MirrorManager::parseMirrorList(QUrl url)
{
    if (gettingMirrorList) {
        theParser.setMirrorList(theNetwork.getData());

        if (url == urls.at(0)) {
            mirrorListAll = theParser.getMirrorList();
            getNextMirrorList(urls.at(1));
        } else if (url == urls.at(1)) {
            mirrorListIPv4 = theParser.getMirrorList();
            getNextMirrorList(urls.at(2));
        } else if (url == urls.at(2)) {
            mirrorListIPv6 = theParser.getMirrorList();
            getNextMirrorList(urls.at(3));
        } else if (url == urls.at(3)) {
            mirrorListStatus = theParser.getMirrorList();
            gettingMirrorList = false;
            processMirrorLists();
        }
    }
}

void MirrorManager::getNextMirrorList(QUrl url)
{
    theNetwork.getRequest(url);
}

void MirrorManager::getMirrorList()
{
    gettingMirrorList = true;
    getNextMirrorList(urls.at(0));
}

void MirrorManager::processMirrorLists()
{
    for (int i = 0; i < mirrorListAll.size(); i++) {
        for (int j = 0; j < mirrorListIPv4.size(); j++) {
            if (mirrorListIPv4.at(j).url == mirrorListAll.at(i).url) {
                mirrorListAll[i].ipv4 = true;
                mirrorListIPv4.removeAt(j);
                break;
            }
        }
        for (int j = 0; j < mirrorListIPv6.size(); j++) {
            if (mirrorListIPv6.at(j).url == mirrorListAll.at(i).url) {
                mirrorListAll[i].ipv6 = true;
                mirrorListIPv6.removeAt(j);
                break;
            }
        }
        for (int j = 0; j < mirrorListStatus.size(); j++) {
            if (mirrorListStatus.at(j).url == mirrorListAll.at(i).url) {
                mirrorListAll[i].status = true;
                mirrorListStatus.removeAt(j);
                break;
            }
        }
    }
    
    emit mirrorListReady(mirrorListAll);
}

void MirrorManager::getCountryList()
{
    gettingCountryList = true;
    theNetwork.getRequest(urls.at(0));
}

void MirrorManager::parseCountryList(QUrl url)
{
    if (gettingCountryList) {
        theParser.setMirrorList(theNetwork.getData());
        emit countryListReady(theParser.getCountryList());
        gettingCountryList = false;
    }
}

QList<mirror> MirrorManager::filterMirrorList(mirrorFilter filter)
{
    if (filter.countryList.isEmpty() &&
        filter.protocolList.contains("http") &&
        filter.protocolList.contains("https") &&
        filter.ipv4 == 1 &&
        filter.ipv6 == 1 &&
        filter.statusOK &&
        filter.statusKO) {
            return mirrorListAll;
    }

    QList<mirror> filteredMirrorList;

    for (int i = 0; i < mirrorListAll.size(); i++) {
        if ((filter.countryList.isEmpty() ||
            filter.countryList.contains(mirrorListAll.at(i).country)) &&
            filter.protocolList.contains(mirrorListAll.at(i).protocol) &&
            (filter.ipv4 == 1 ||
            (filter.ipv4 == 2 && mirrorListAll.at(i).ipv4) ||
            (filter.ipv4 == 0 && !mirrorListAll.at(i).ipv4)) &&
            (filter.ipv6 == 1 ||
            (filter.ipv6 == 2 && mirrorListAll.at(i).ipv6) ||
            (filter.ipv6 == 0 && !mirrorListAll.at(i).ipv6)) &&
            ((filter.statusOK && mirrorListAll.at(i).status) ||
            (filter.statusKO && !mirrorListAll.at(i).status))) {
                filteredMirrorList.append(mirrorListAll.at(i));
        }
    }
    
    return filteredMirrorList;
}
