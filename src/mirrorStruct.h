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

#ifndef MIRRORSTRUCT_H
#define MIRRORSTRUC_H

#include <QString>
#include <QDateTime>
#include <QPixmap>

struct Mirror
{
    QString url;
    QString protocol;
    QDateTime last_sync;
    double completion_pct;
    int delay;
    double duration_avg;
    double duration_stddev;
    double score;
    bool active;
    QString country;
    QString country_code;
    bool isos;
    bool ipv4;
    bool ipv6;
    bool selected;
    QPixmap flag;
};

struct Country
{
    QString name;
    QString code;
    QPixmap flag;
};

#endif
