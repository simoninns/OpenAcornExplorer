/************************************************************************

    adfsdirectory.cpp

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

#include "adfsdirectory.h"

AdfsDirectory::AdfsDirectory()
{
    directoryData = new QByteArray;
    sectorSize = 256;
}

// Note: Should this also need the sector size?
bool AdfsDirectory::setDirectory(QByteArray directoryDataParam)
{
    bool directoryValid = false;

    // Copy the directory data into the object
    directoryData->clear();
    directoryData->append(directoryDataParam);

    // Check the directory identification string
    qDebug() << "AdfsDirectory::setDirectory(): Directory identification string is" << getIdentificationString();

    if (QString::compare(getIdentificationString(), "Hugo", Qt::CaseSensitive) != 0) {
        // Not a valid ADFS directory
        directoryValid = false;
        qDebug() << "AdfsDirectory::setDirectory(): Error, directory identification string is invalid!";
    } else {
        // Valid directory
        directoryValid = true;
    }

    return directoryValid;
}

qint64 AdfsDirectory::getMasterSequenceNumber()
{
    // Value is stored as binary-coded decimal
    return convertBcdToInt(directoryData->at(0)); // Also at 1274
}

QString AdfsDirectory::getIdentificationString()
{
    // Check the directory identification string
    QString startIdentificationString;
    QString endIdentificationString;

    startIdentificationString = QString::fromLatin1(directoryData->mid(1, 4));
    endIdentificationString = QString::fromLatin1(directoryData->mid(1275, 4));

    // Ensure that both strings match
    if (QString::compare(startIdentificationString, endIdentificationString, Qt::CaseSensitive) != 0) {
        // Identification strings are not the same!
        startIdentificationString.clear();
    }

    return startIdentificationString;
}

QString AdfsDirectory::getEntryName(qint64 entryNumber)
{
    // Entry name starts at byte 5 (each entry is 26 bytes in total)
    qint64 entryOffset = 5 + (entryNumber * 26);

    QByteArray entryNameAndAccess;
    entryNameAndAccess = directoryData->mid(entryOffset, 10);

    // Strip the directory access attributes from the directory name
    entryNameAndAccess[0] = entryNameAndAccess[0] & 0x7F;
    entryNameAndAccess[1] = entryNameAndAccess[1] & 0x7F;
    entryNameAndAccess[2] = entryNameAndAccess[2] & 0x7F;
    entryNameAndAccess[3] = entryNameAndAccess[3] & 0x7F;

    return getTerminatedString(entryNameAndAccess, 10);
}

// Function to return the read flag of a directory entry
bool AdfsDirectory::isEntryReadable(qint64 entryNumber)
{
    bool flag = false;

    // Entry access starts at byte 5 (each entry is 26 bytes in total)
    qint64 entryOffset = 5 + (entryNumber * 26);

    QByteArray entryNameAndAccess;
    entryNameAndAccess = directoryData->mid(entryOffset, 4);
    if ((entryNameAndAccess[0] & 0x80) == 0x80) flag = true;

    return flag;
}

// Function to return the write flag of a directory entry
bool AdfsDirectory::isEntryWritable(qint64 entryNumber)
{
    bool flag = false;

    // Entry access starts at byte 5 (each entry is 26 bytes in total)
    qint64 entryOffset = 5 + (entryNumber * 26);

    QByteArray entryNameAndAccess;
    entryNameAndAccess = directoryData->mid(entryOffset, 4);
    if ((entryNameAndAccess[1] & 0x80) == 0x80) flag = true;

    return flag;
}

// Function to return the lock flag of a directory entry
bool AdfsDirectory::isEntryLocked(qint64 entryNumber)
{
    bool flag = false;

    // Entry access starts at byte 5 (each entry is 26 bytes in total)
    qint64 entryOffset = 5 + (entryNumber * 26);

    QByteArray entryNameAndAccess;
    entryNameAndAccess = directoryData->mid(entryOffset, 4);
    if ((entryNameAndAccess[2] & 0x80) == 0x80) flag = true;

    return flag;
}

// Function to return the directory flag of a directory entry
// Note: true = entry is a directory, false = entry is a file
bool AdfsDirectory::isEntryDirectory(qint64 entryNumber)
{
    bool flag = false;

    // Entry access starts at byte 5 (each entry is 26 bytes in total)
    qint64 entryOffset = 5 + (entryNumber * 26);

    QByteArray entryNameAndAccess;
    entryNameAndAccess = directoryData->mid(entryOffset, 4);
    if ((entryNameAndAccess[3] & 0x80) == 0x80) flag = true;

    return flag;
}

qint64 AdfsDirectory::getEntryLoadAddress(qint64 entryNumber)
{
    // Entry load address starts at byte 15 (each entry is 26 bytes in total)
    qint64 entryOffset = 15 + (entryNumber * 26);

    return convertBytesToInt(directoryData->at(3 + entryOffset),
                             directoryData->at(2 + entryOffset),
                             directoryData->at(1 + entryOffset),
                             directoryData->at(0 + entryOffset));
}

qint64 AdfsDirectory::getEntryExecutionAddress(qint64 entryNumber)
{
    // Entry execution address starts at byte 19 (each entry is 26 bytes in total)
    qint64 entryOffset = 19 + (entryNumber * 26);

    return convertBytesToInt(directoryData->at(3 + entryOffset),
                             directoryData->at(2 + entryOffset),
                             directoryData->at(1 + entryOffset),
                             directoryData->at(0 + entryOffset));
}

qint64 AdfsDirectory::getEntryLength(qint64 entryNumber)
{
    // Entry length starts at byte 23 (each entry is 26 bytes in total)
    qint64 entryOffset = 23 + (entryNumber * 26);

    return convertBytesToInt(directoryData->at(3 + entryOffset),
                             directoryData->at(2 + entryOffset),
                             directoryData->at(1 + entryOffset),
                             directoryData->at(0 + entryOffset));
}

qint64 AdfsDirectory::getEntryStartSector(qint64 entryNumber)
{
    // Entry start sector starts at byte 27 (each entry is 26 bytes in total)
    qint64 entryOffset = 27 + (entryNumber * 26);

    return convertBytesToInt(directoryData->at(2 + entryOffset),
                             directoryData->at(1 + entryOffset),
                             directoryData->at(0 + entryOffset));
}

qint64 AdfsDirectory::getEntrySequenceNumber(qint64 entryNumber)
{
    // Entry name starts at byte 30 (each entry is 26 bytes in total)
    qint64 entryOffset = 30 + (entryNumber * 26);

    // Value is stored as binary-coded decimal
    return convertBcdToInt(directoryData->at(entryOffset));
}

QString AdfsDirectory::getDirectoryName()
{
    QByteArray directoryNameAndAccess;
    directoryNameAndAccess = directoryData->mid(1228, 10);

    // Strip the directory access attributes from the directory name
    directoryNameAndAccess[0] = directoryNameAndAccess[0] & 0x7F;
    directoryNameAndAccess[1] = directoryNameAndAccess[1] & 0x7F;
    directoryNameAndAccess[2] = directoryNameAndAccess[2] & 0x7F;
    directoryNameAndAccess[3] = directoryNameAndAccess[3] & 0x7F;

    return getTerminatedString(directoryNameAndAccess, 10);
}

// Function to return the read flag of the directory
bool AdfsDirectory::isDirectoryReadable()
{
    bool flag = false;

    QByteArray directoryNameAndAccess;
    directoryNameAndAccess = directoryData->mid(1228, 4);
    if ((directoryNameAndAccess[0] & 0x80) == 0x80) flag = true;

    return flag;
}

// Function to return the write flag of the directory
bool AdfsDirectory::isDirectoryWritable()
{
    bool flag = false;

    QByteArray directoryNameAndAccess;
    directoryNameAndAccess = directoryData->mid(1228, 4);
    if ((directoryNameAndAccess[1] & 0x80) == 0x80) flag = true;

    return flag;
}

// Function to return the lock flag of the directory
bool AdfsDirectory::isDirectoryLocked()
{
    bool flag = false;

    QByteArray directoryNameAndAccess;
    directoryNameAndAccess = directoryData->mid(1228, 4);
    if ((directoryNameAndAccess[2] & 0x80) == 0x80) flag = true;

    return flag;
}

QString AdfsDirectory::getDirectoryTitle()
{
    QByteArray title;
    title = directoryData->mid(1241, 19);

    return getTerminatedString(title, 19);
}

// Private methods

// Takes QByteArray data containing string and detects either termination
// or maximum allowed length - returns a QString result
QString AdfsDirectory::getTerminatedString(QByteArray data, qint64 maximumLength)
{
    // Handle empty string and ASCII CR termination
    qint64 stringLength = maximumLength;
    if (data.at(0) != 0x00 && data.at(0) != 0x0D) {
        for (qint64 pointer = 0; pointer < maximumLength; pointer++) {
            if (data.at(pointer) == 0x0D  || data.at(pointer) == 0x00) {
                stringLength = pointer;
                break;
            }
        }
    } else {
        // Return empty string
        return QString("");
    }

    return QString::fromLatin1(data, stringLength);
}

// Convert 4 bytes to an integer (byte0 is MSB, byte 3 is LSB)
qint64 AdfsDirectory::convertBytesToInt(quint8 byte0, quint8 byte1, quint8 byte2, quint8 byte3)
{
    quint32 value = 0;

    value = byte0 << 24;
    value += byte1 << 16;
    value += byte2 << 8;
    value += byte3;

    return (qint64)value;
}

// Convert 3 bytes to an integer (byte0 is MSB, byte 2 is LSB)
qint64 AdfsDirectory::convertBytesToInt(quint8 byte0, quint8 byte1, quint8 byte2)
{
    quint32 value = 0;

    value = byte0 << 16;
    value += byte1 << 8;
    value += byte2;

    return (qint64)value;
}

// Convert BCD to integer
qint64 AdfsDirectory::convertBcdToInt(quint8 byte0)
{
    return (qint64)(((byte0 >> 4) * 10) + (byte0 & 0x0F));
}

// Convert integer to BCD
quint8 AdfsDirectory::convertIntToBcd(qint64 byte0)
{
    if (byte0 > 99) byte0 = 99;
    if (byte0 < 0) byte0 = 0;

    return (quint8)(((byte0/10) << 4) | (byte0 % 10));
}
