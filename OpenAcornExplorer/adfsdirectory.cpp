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

    // Copy the free space map data into the object
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
    quint8 masterSequenceNumber;

    // Note: This is in BCD, so this won't work as-is!
    masterSequenceNumber = directoryData->at(0); // Also present at 1274

    return (qint64)masterSequenceNumber;
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
    qint64 entryOffset = 5 + (entryNumber * 26); // 5 bytes before entries, each entry is 26 bytes

    QByteArray entryNameAndAccess;
    entryNameAndAccess = directoryData->mid(entryOffset, 10);

    // Strip the directory access attributes from the directory name
    entryNameAndAccess[0] = entryNameAndAccess[0] & 0x7F;
    entryNameAndAccess[1] = entryNameAndAccess[1] & 0x7F;
    entryNameAndAccess[2] = entryNameAndAccess[2] & 0x7F;
    entryNameAndAccess[3] = entryNameAndAccess[3] & 0x7F;

    return getTerminatedString(entryNameAndAccess, 10);
}

qint64 AdfsDirectory::getEntryAccess(qint64 entryNumber)
{
    qint64 entryOffset = 5 + (entryNumber * 26); // 5 bytes before entries, each entry is 26 bytes

    quint8 accessFlags = 0;
    QByteArray entryNameAndAccess;
    entryNameAndAccess = directoryData->mid(entryOffset, 4);

    // Strip the directory name from the access attributes
    entryNameAndAccess[0] = entryNameAndAccess[0] & 0x80;
    entryNameAndAccess[1] = entryNameAndAccess[1] & 0x80;
    entryNameAndAccess[2] = entryNameAndAccess[2] & 0x80;
    entryNameAndAccess[3] = entryNameAndAccess[3] & 0x80;

    if ((entryNameAndAccess[0] & 0x80) == 0x80) accessFlags += 1;
    if ((entryNameAndAccess[1] & 0x80) == 0x80) accessFlags += 2;
    if ((entryNameAndAccess[2] & 0x80) == 0x80) accessFlags += 4;
    if ((entryNameAndAccess[3] & 0x80) == 0x80) accessFlags += 8;

    return (qint64)accessFlags;
}

qint64 AdfsDirectory::getEntryLoadAddress(qint64 entryNumber)
{
    qint64 entryOffset = 15 + (entryNumber * 26); // 15 bytes before entries, each entry is 26 bytes

    return convertBytesToInt(directoryData->at(3 + entryOffset),
                             directoryData->at(2 + entryOffset),
                             directoryData->at(1 + entryOffset),
                             directoryData->at(0 + entryOffset));
}

qint64 AdfsDirectory::getEntryExecutionAddress(qint64 entryNumber)
{
    qint64 entryOffset = 19 + (entryNumber * 26); // 15 bytes before entries, each entry is 26 bytes

    return convertBytesToInt(directoryData->at(3 + entryOffset),
                             directoryData->at(2 + entryOffset),
                             directoryData->at(1 + entryOffset),
                             directoryData->at(0 + entryOffset));
}

qint64 AdfsDirectory::getEntryLength(qint64 entryNumber)
{
    qint64 entryOffset = 23 + (entryNumber * 26); // 15 bytes before entries, each entry is 26 bytes

    return convertBytesToInt(directoryData->at(3 + entryOffset),
                             directoryData->at(2 + entryOffset),
                             directoryData->at(1 + entryOffset),
                             directoryData->at(0 + entryOffset));
}

qint64 AdfsDirectory::getEntryStartSector(qint64 entryNumber)
{
    qint64 entryOffset = 27 + (entryNumber * 26); // 15 bytes before entries, each entry is 26 bytes

    return convertBytesToInt(directoryData->at(3 + entryOffset),
                             directoryData->at(2 + entryOffset),
                             directoryData->at(1 + entryOffset),
                             directoryData->at(0 + entryOffset));
}

qint64 AdfsDirectory::getEntrySequenceNumber(qint64 entryNumber)
{
    qint64 entryOffset = 30 + (entryNumber * 26); // 15 bytes before entries, each entry is 26 bytes
    return directoryData->at(entryOffset);
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

qint64 AdfsDirectory::getDirectoryAccess()
{
    quint8 accessFlags = 0;
    QByteArray directoryNameAndAccess;
    directoryNameAndAccess = directoryData->mid(1228, 4);

    // Strip the directory name from the access attributes
    directoryNameAndAccess[0] = directoryNameAndAccess[0] & 0x80;
    directoryNameAndAccess[1] = directoryNameAndAccess[1] & 0x80;
    directoryNameAndAccess[2] = directoryNameAndAccess[2] & 0x80;
    directoryNameAndAccess[3] = directoryNameAndAccess[3] & 0x80;

    if ((directoryNameAndAccess[0] & 0x80) == 0x80) accessFlags += 1;
    if ((directoryNameAndAccess[1] & 0x80) == 0x80) accessFlags += 2;
    if ((directoryNameAndAccess[2] & 0x80) == 0x80) accessFlags += 4;
    if ((directoryNameAndAccess[3] & 0x80) == 0x80) accessFlags += 8;

    return (qint64)accessFlags;
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
