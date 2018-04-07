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
    // The root directory is stored in sectors 2 to 6 of all ADFS discs
    QByteArray directoryData;
    directoryData.resize(5 * adfsDiscImage->getSectorSize());

    // Read 6 sectors of directory data
    qDebug() << "AdfsImage::readDirectory(): Reading directory data";
    directoryData = adfsDiscImage->readSector(sector, sector + 5);
    qDebug() << "AdfsImage::readDirectory(): Reading directory data complete";

    // Create the directory object (test only!)
    AdfsDirectory adfsDirectory;

    // Put the free space map data into the object
    if (adfsDirectory.setDirectory(directoryData)) {
        qDebug() << "Root directory is valid";
        qDebug() << "Directory master sequence number is" << adfsDirectory.getMasterSequenceNumber();
        qDebug() << "Directory identification string is" << adfsDirectory.getIdentificationString();
        qDebug() << "Directory name is" << adfsDirectory.getDirectoryName();

        QString dirAttributes;
        if (adfsDirectory.isDirectoryReadable()) dirAttributes.append("R");
        if (adfsDirectory.isDirectoryWritable()) dirAttributes.append("W");
        if (adfsDirectory.isDirectoryLocked()) dirAttributes.append("L");
        qDebug() << "Directory has attributes" << dirAttributes;

        qDebug() << "Directory title is" << adfsDirectory.getDirectoryTitle();
        qDebug() << "Listing directory entries:";

        // List entries in the directory (maximum of 47 entries)
        for (qint64 entryNumber = 0; entryNumber < 47; entryNumber++) {
            // Check for last entry
            if (adfsDirectory.getEntryName(entryNumber).isEmpty()) {
                // Last entry
                qDebug() << "  End of directory entries";
                break;
            }

            // List entry details
            qDebug() << "  Entry" << entryNumber << "name is" << adfsDirectory.getEntryName(entryNumber);

            QString entryAttributes;
            if (adfsDirectory.isEntryDirectory(entryNumber)) entryAttributes.append("D"); else entryAttributes.append("F");
            if (adfsDirectory.isEntryReadable(entryNumber)) entryAttributes.append("R");
            if (adfsDirectory.isEntryWritable(entryNumber)) entryAttributes.append("W");
            if (adfsDirectory.isEntryLocked(entryNumber)) entryAttributes.append("L");
            qDebug() << "  Entry" << entryNumber << "has attributes" << entryAttributes;

            qDebug() << "  Entry" << entryNumber << "load address is 0x" << QString::number(adfsDirectory.getEntryLoadAddress(entryNumber), 16);
            qDebug() << "  Entry" << entryNumber << "execution address is 0x" << QString::number(adfsDirectory.getEntryExecutionAddress(entryNumber), 16);
            qDebug() << "  Entry" << entryNumber << "length is 0x" << QString::number(adfsDirectory.getEntryLength(entryNumber), 16);
            qDebug() << "  Entry" << entryNumber << "start sector is 0x" << QString::number(adfsDirectory.getEntryStartSector(entryNumber), 16);
            qDebug() << "  Entry" << entryNumber << "sequence number is" << adfsDirectory.getEntrySequenceNumber(entryNumber);
        }
    } else {
        // Root directory is not valid!
        qDebug() << "Root directory is not valid!";
    }
}

