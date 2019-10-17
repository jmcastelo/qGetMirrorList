#ifndef MIRRORMANAGER_H
#define MIRRORMANAGER_H

#include "network.h"
#include "mirrorParser.h"
#include <QObject>
#include <QList>
#include <QUrl>

struct mirrorFilter
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

        QList<mirror> filterMirrorList(mirrorFilter filter);

    signals:
        void mirrorListReady(QList<mirror> mirrorList);
        void countryListReady(QList<Country> countryList);

    private slots:
        void parseMirrorList(QUrl url);
        void parseCountryList(QUrl url);

    private:
        Network theNetwork;
        MirrorParser theParser;
        QList<mirror> mirrorListAll;
        QList<mirror> mirrorListIPv4;
        QList<mirror> mirrorListIPv6;
        QList<mirror> mirrorListStatus;
        QList<QUrl> urls;
        bool gettingMirrorList;
        bool gettingCountryList;
        void getNextMirrorList(QUrl url);
        void processMirrorLists();
};

#endif
