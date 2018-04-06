/************************************************************************

    adfsfreespacemap.cpp

    OpenAcornExplorer - Acorn 8-bit and 32-bit disc image manipulation
    Copyright (C) 2018 Simon Inns

    This file is part of OpenAcornExplorer.

    OpenAcornExplorer is free software: you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Email: simon.inns@gmail.com

************************************************************************/

#ifndef ADFSFREESPACEMAP_H
#define ADFSFREESPACEMAP_H

#include <QApplication>
#include <QDebug>

class AdfsFreeSpaceMap
{
public:
    AdfsFreeSpaceMap();
    bool setMap(QByteArray freeSpaceMapDataParam);

    qint64 getFreeSpaceStartSector(qint64 freeSpaceNumber);
    qint64 getFreeSpaceLength(qint64 freeSpaceNumber);
    qint64 getTotalSectorsOnDisc();
    qint64 getDiscIdentifier();
    qint64 getBootOptionNumber();

private:
    QByteArray *freeSpaceMapData;
    qint64 sectorSize;

    qint64 convertBytesToInt(quint8 byte0, quint8 byte1);
    qint64 convertBytesToInt(quint8 byte0, quint8 byte1, quint8 byte2);
    qint64 calculateChecksum(qint64 sectorNumber);
};

#endif // ADFSFREESPACEMAP_H
