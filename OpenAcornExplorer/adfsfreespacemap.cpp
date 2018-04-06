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

#include "adfsfreespacemap.h"

AdfsFreeSpaceMap::AdfsFreeSpaceMap()
{
    freeSpaceMapData = new QByteArray;
    sectorSize = 256;
}

// Note: Should this also need the sector size?
bool AdfsFreeSpaceMap::setMap(QByteArray freeSpaceMapDataParam)
{
    bool freeSpaceMapValid = false;

    // Copy the free space map data into the object
    freeSpaceMapData->clear();
    freeSpaceMapData->append(freeSpaceMapDataParam);

    // Get the free space map checksums from the free space map
    quint8 discChecksumSector0 = freeSpaceMapData->at(255);
    quint8 discChecksumSector1 = freeSpaceMapData->at(sectorSize + 255);

    // Calculate the free space map checksums
    quint8 calcChecksumSector0 = calculateChecksum(0);
    quint8 calcChecksumSector1 = calculateChecksum(1);

    // Do the checksums match?
    if (discChecksumSector0 == calcChecksumSector0 && discChecksumSector1 == calcChecksumSector1) {
        // Checksums are valid
        freeSpaceMapValid = true;
    } else {
        // Checksums are not valid
        if (discChecksumSector0 != calcChecksumSector0)
            qDebug() << "AdfsFreeSpaceMap::setMap(): Error, sector 0 checksum is invalid!";

        if (discChecksumSector1 != calcChecksumSector1)
            qDebug() << "AdfsFreeSpaceMap::setMap(): Error, sector 1 checksum is invalid!";
    }

    return freeSpaceMapValid;
}

qint64 AdfsFreeSpaceMap::getFreeSpaceStartSector(qint64 freeSpaceNumber)
{
    qint64 offset = freeSpaceNumber * 3; // 3 bytes per record - maximum of 82 entries

    return convertBytesToInt(freeSpaceMapData->at(offset + 2), freeSpaceMapData->at(offset + 1), freeSpaceMapData->at(offset));
}

qint64 AdfsFreeSpaceMap::getFreeSpaceLength(qint64 freeSpaceNumber)
{
    // Stored in sector 1
    qint64 offset = 255 + (freeSpaceNumber * 3); // 3 bytes per record - maximum of 82 entries

    // Returned free space length is in number of sectors
    return convertBytesToInt(freeSpaceMapData->at(offset + 2), freeSpaceMapData->at(offset + 1), freeSpaceMapData->at(offset));
}

qint64 AdfsFreeSpaceMap::getTotalSectorsOnDisc()
{
    // Bytes 252-254 of sector 0
    return convertBytesToInt(freeSpaceMapData->at(254), freeSpaceMapData->at(253), freeSpaceMapData->at(252));
}

qint64 AdfsFreeSpaceMap::getDiscIdentifier()
{
    // Bytes 251-252 of sector 1
    return convertBytesToInt(freeSpaceMapData->at(sectorSize + 252), freeSpaceMapData->at(sectorSize + 251));
}

qint64 AdfsFreeSpaceMap::getBootOptionNumber()
{
    // Byte 253 of sector 1
    return freeSpaceMapData->at(sectorSize + 253);
}

// Private methods

// Convert 2 bytes to an integer (byte0 is MSB, byte 1 is LSB)
qint64 AdfsFreeSpaceMap::convertBytesToInt(quint8 byte0, quint8 byte1)
{
    quint16 value = 0;

    value = byte0 << 8;
    value += byte1;

    return (qint64)value;
}

// Convert 3 bytes to an integer (byte0 is MSB, byte 2 is LSB)
qint64 AdfsFreeSpaceMap::convertBytesToInt(quint8 byte0, quint8 byte1, quint8 byte2)
{
    quint32 value = 0;

    value = byte0 << 16;
    value += byte1 << 8;
    value += byte2;

    return (qint64)value;
}

// Calculate the ADFS free space map sector checksum
qint64 AdfsFreeSpaceMap::calculateChecksum(qint64 sectorNumber)
{
    quint16 sum = 255;

    for (qint64 pointer = (sectorSize - 2); pointer >= 0; pointer--) {
        if (sum > 255) sum = (sum + 1) & 0xFF;
        sum += (quint16)freeSpaceMapData->at((sectorNumber * sectorSize) + pointer);
    }
    sum &= 0xFF;

    return (qint64)sum;
}
