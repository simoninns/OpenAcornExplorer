/************************************************************************

    discimage.h

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

#ifndef DISCIMAGE_H
#define DISCIMAGE_H

#include <QApplication>
#include <QDebug>
#include <QFile>

class DiscImage
{
public:
    DiscImage(QString filename);
    ~DiscImage();

    QByteArray readSector(qint64 sectorNumber);
    QByteArray readSector(qint64 startSectorNumber, qint64 numberOfSectors);

    qint64 getSectorSize();
    bool isValid();

private:
    QFile *discImageFile;
    bool discImageOpen;

    // Disc geometry
    qint64 tracks;
    qint64 sides;
    qint64 sectorsPerTrack;
    qint64 sectorSize;
    qint64 startSector;

    qint64 translateSectorToByte(qint64 sector);
};

#endif // DISCIMAGE_H
