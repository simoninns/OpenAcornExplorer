/************************************************************************

    discimage.cpp

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

#include "discimage.h"

// Class constructor
DiscImage::DiscImage(QString filename)
{
    // Set the default image attributes (ADFS L format only!)
    tracks = 80; // Tracks per side/head
    sides = 2; // Number of sides/heads
    sectorsPerTrack = 16; // Number of sectors per track
    sectorSize = 256;
    startSector = 0; // Number of first sector

    // Open the file
    discImageOpen = false;
    discImageFile = new QFile(filename);

    if (!discImageFile->open(QIODevice::ReadWrite)) {
        qDebug() << "DiscImage::DiscImage(): Failed to open disc image file";
    }

    // Disc image file opened successfully
    discImageOpen = true;
}

// Class destructor
DiscImage::~DiscImage()
{
    // Is there an open file?
    if (discImageOpen) {
        qDebug() << "DiscImage::~DiscImage(): Closing disc image file";
        discImageOpen = false;
        discImageFile->close();
    }
}

// Read a single sector from a disc image
QByteArray DiscImage::readSector(qint64 sectorNumber)
{
    QByteArray sectorData;
    sectorData.resize(sectorSize);

    // Check that a disc image has been successfully opened
    if (!discImageOpen) {
        qDebug() << "DiscImage::readSector(S): Disc image is not open!";
        return sectorData; // Returns an empty sector
    }

    // Seek to the required sector
    if (!discImageFile->seek(translateSectorToByte(sectorNumber))) {
        // Seek operation failed...
        qDebug() << "DiscImage::readSector(S): Could not seek to sector" << sectorNumber;
        return sectorData; // Returns an empty sector
    }

    qint64 bytesRead = 0;
    bytesRead = discImageFile->read(sectorData.data(), sectorSize);

    // Was read operation successful?
    if (bytesRead == -1) {
        qDebug() << "DiscImage::readSector(S): Could not read sector" << sectorNumber;
    }

    return sectorData;
}

// Read multiple sectors from a disc image
QByteArray DiscImage::readSector(qint64 startSectorNumber, qint64 numberOfSectors)
{
    QByteArray sectorData;

    // Check that a disc image has been successfully opened
    if (!discImageOpen) {
        qDebug() << "DiscImage::readSector(M): Disc image is not open!";
        return sectorData; // Returns an empty sector
    }

    // Read the required sectors from the disc image
    for (qint64 sectorOffset = 0; sectorOffset < numberOfSectors; sectorOffset++) {
        sectorData.append(readSector(startSectorNumber + sectorOffset));
    }

    return sectorData;
}

// Get and set methods

// Get the current sector size
qint64 DiscImage::getSectorSize()
{
    return sectorSize;
}

// Determine if the disc image is valid
bool DiscImage::isValid()
{
    if (!discImageOpen) return false;

    return true;
}

// Private methods ----------------------------------------------------------------------------------------------------

// Translate a sector number to a byte position
qint64 DiscImage::translateSectorToByte(qint64 sector)
{
    // Which disc side is the required sector on?
    qint64 side = sector / (tracks * sectorsPerTrack);

    // Calculate the actual byte location in the file, compensating
    // for track interleave on a double-sided disc image
    qint64 imageOffset = (sector % sectorsPerTrack) +
            (2 * sectorsPerTrack * (sector / sectorsPerTrack)) +
            (sectorsPerTrack * side);

    return imageOffset * sectorSize;
}
