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
DiscImage::DiscImage()
{
    // Set the default image attributes
    interleavedFlag = false;
    sectorSize = 256;
    discImageOpen = false;
}

// Class destructor
DiscImage::~DiscImage()
{
    // Is there an open file?
    if (discImageOpen) {
        qDebug() << "DiscImage::~DiscImage(): Closing disc image file";
        close();
    }
}

bool DiscImage::open(QString filename)
{
    // Open the file
    discImageOpen = false;
    discImageFile = new QFile(filename);

    if (!discImageFile->open(QIODevice::ReadWrite)) {
        qDebug() << "DiscImage::DiscImage(): Failed to open disc image file";
        return false;
    }

    // Disc image file opened successfully
    discImageOpen = true;
    fileBytePosition = 0;
    return true;
}

void DiscImage::close()
{
    // Is there an open file?
    if (discImageOpen) {
        qDebug() << "DiscImage::~DiscImage(): Closing disc image file";
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

    // Read a sector of data starting from startByte
    qDebug() << "DiscImage::readSector(S): Reading sector" << sectorNumber;

    // Check the current file position and seek if necessary
    if (fileBytePosition != translateSectorToByte(sectorNumber)) {
        if (!discImageFile->seek(translateSectorToByte(sectorNumber))) {
            // Seek operation failed...
            qDebug() << "DiscImage::readSector(S): Could not seek to sector" << sectorNumber;
            return sectorData; // Returns an empty sector
        }
        fileBytePosition = translateSectorToByte(sectorNumber);
    }

    qint64 bytesRead = 0;
    bytesRead = discImageFile->read(sectorData.data(), sectorSize);

    // Was read operation successful?
    if (bytesRead == -1) {
        qDebug() << "DiscImage::readSector(S): Could not read sector" << sectorNumber;
    } else {
        // Successful - update file position pointer
        fileBytePosition += bytesRead;
    }

    return sectorData;
}

// Read multiple sectors from a disc image
QByteArray DiscImage::readSector(qint64 startSectorNumber, qint64 endSectorNumber)
{
    QByteArray sectorData;
    qint64 numberOfSectors = endSectorNumber - startSectorNumber + 1;

    sectorData.resize(sectorSize * numberOfSectors);

    // Check that a disc image has been successfully opened
    if (!discImageOpen) {
        qDebug() << "DiscImage::readSector(M): Disc image is not open!";
        return sectorData; // Returns an empty sector
    }

    // Read a sector of data starting from startByte
    qDebug() << "DiscImage::readSector(M): Reading sectors" << startSectorNumber << "to" << endSectorNumber;

    // Check the current file position and seek if necessary
    if (fileBytePosition != translateSectorToByte(startSectorNumber)) {
        if (!discImageFile->seek(translateSectorToByte(startSectorNumber))) {
            // Seek operation failed...
            qDebug() << "DiscImage::readSector(M): Could not seek to sector" << startSectorNumber;
            return sectorData; // Returns an empty sector
        }
        fileBytePosition = translateSectorToByte(startSectorNumber);
    }

    qint64 bytesRead = 0;
    qDebug() << "DiscImage::readSector(M): Reading from disc image";
    bytesRead = discImageFile->read(sectorData.data(), sectorSize * numberOfSectors);
    qDebug() << "DiscImage::readSector(M): Reading complete";

    // Was read operation successful?
    if (bytesRead == -1) {
        qDebug() << "DiscImage::readSector(M): Could not read sectors from" << startSectorNumber;
    } else {
        // Successful - update file position pointer
        fileBytePosition += bytesRead;
    }

    return sectorData;
}

// Write a sector from a disc image
bool DiscImage::writeSector(qint64 sectorNumber, QByteArray sectorData)
{
    // Check that a disc image has been successfully opened
    if (!discImageOpen) {
        qDebug() << "DiscImage::writeSector(): Disc image is not open!";
        return false;
    }

    // Write a sector of data starting from startByte
    qDebug() << "DiscImage::writeSector(): Writing sector" << sectorNumber;

    // Check the current file position and seek if necessary
    if (fileBytePosition != translateSectorToByte(sectorNumber)) {
        if (!discImageFile->seek(translateSectorToByte(sectorNumber))) {
        // Seek operation failed...
            qDebug() << "DiscImage::writeSector(): Could not seek to sector" << sectorNumber;
            return false;
        }
        fileBytePosition = translateSectorToByte(sectorNumber);
    }

    qint64 bytesWritten = 0;
    bytesWritten = discImageFile->write(sectorData.data(), sectorSize);

    // Was write operation successful?
    if (bytesWritten == -1) {
        qDebug() << "DiscImage::writeSector(): Could not write sector" << sectorNumber;
        return false;
    } else {
        // Successful - update file position pointer
        fileBytePosition += bytesWritten;
    }

    return true;
}

// Get and set methods
void DiscImage::setSectorSize(qint64 sectorSizeParam)
{
    // Sanity check the sector size
    if (sectorSizeParam != 256 && sectorSizeParam != 512) sectorSizeParam = 256;

    sectorSize = sectorSizeParam;
}

qint64 DiscImage::getSectorSize()
{
    return sectorSize;
}

void DiscImage::setInterleavedFlag(bool interleavedParam)
{
    interleavedFlag = interleavedParam;
}

// Private methods

// Translate a sector number to a byte position
qint64 DiscImage::translateSectorToByte(qint64 sectorNumber)
{
    return sectorNumber * sectorSize;
}
