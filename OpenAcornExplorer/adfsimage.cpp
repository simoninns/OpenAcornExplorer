/************************************************************************

    adfsimage.cpp

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

#include "adfsimage.h"

AdfsImage::AdfsImage()
{
    adfsDiscImage = new DiscImage;
}

bool AdfsImage::open(QString filename)
{
    if (!adfsDiscImage->open(filename)) {
        qDebug() << "AdfsImage::open(): Error opening ADFS disc image";
        return false;
    }

    // Image opened successfully
    adfsDiscImage->setInterleavedFlag(false);
    adfsDiscImage->setSectorSize(256);

    return true;
}

void AdfsImage::close()
{
    adfsDiscImage->close();
}

void AdfsImage::readFreeSpaceMap()
{
    // The free space map is stored in sectors 0 and 1 of all ADFS discs
    QByteArray freeSpaceMapData;
    freeSpaceMapData.resize(2 * adfsDiscImage->getSectorSize());

    // Read sectors 0 to 1
    qDebug() << "AdfsImage::readFreeSpaceMap(): Reading free space map data";
    freeSpaceMapData = adfsDiscImage->readSector(0, 1);
    qDebug() << "AdfsImage::readFreeSpaceMap(): Reading free space map data complete";

    // Create the free space map object (test only!)
    AdfsFreeSpaceMap freeSpaceMap;

    // Put the free space map data into the object
    if (freeSpaceMap.setMap(freeSpaceMapData)) {
        qDebug() << "Start sector of free space 00 is" << freeSpaceMap.getFreeSpaceStartSector(0);
        qDebug() << "Start sector of free space 81 is" << freeSpaceMap.getFreeSpaceStartSector(81);
        qDebug() << "Total number of sectors on disc is" << freeSpaceMap.getTotalSectorsOnDisc();
        qDebug() << "Disc identifier is" << freeSpaceMap.getDiscIdentifier();
        qDebug() << "Boot option number is" << freeSpaceMap.getBootOptionNumber();
    } else {
        // Free space map is not valid!
        qDebug() << "Free space map is not valid!";
    }
}

void AdfsImage::readDirectory(qint64 sector)
{

}

