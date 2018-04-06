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

#ifndef ADFSDIRECTORY_H
#define ADFSDIRECTORY_H

#include <QApplication>
#include <QDebug>

class AdfsDirectory
{
public:
    AdfsDirectory();

    bool setDirectory(QByteArray directoryDataParam);

    qint64 getMasterSequenceNumber();
    QString getIdentificationString();
    QString getEntryName(qint64 entryNumber);
    qint64 getEntryAccess(qint64 entryNumber);
    qint64 getEntryLoadAddress(qint64 entryNumber);
    qint64 getEntryExecutionAddress(qint64 entryNumber);
    qint64 getEntryLength(qint64 entryNumber);
    qint64 getEntryStartSector(qint64 entryNumber);
    qint64 getEntrySequenceNumber(qint64 entryNumber);
    QString getDirectoryName();
    qint64 getDirectoryAccess();
    QString getDirectoryTitle();

private:
    QByteArray *directoryData;
    qint64 sectorSize;
};

#endif // ADFSDIRECTORY_H
