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
