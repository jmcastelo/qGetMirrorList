#ifndef MIRRORSTRUCT_H
#define MIRRORSTRUC_H

#include <QString>
#include <QPixmap>

struct mirror
{
    QString url;
    QString country;
    QString protocol;
    bool ipv4;
    bool ipv6;
    bool status;
    bool selected;
    QPixmap flag;
};

struct Country
{
    QString name;
    QPixmap flag;
};

#endif
