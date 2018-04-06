/************************************************************************

    adfsimage.h

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

#ifndef ADFSIMAGE_H
#define ADFSIMAGE_H

#include "discimage.h"
#include "adfsfreespacemap.h"

#include <QApplication>
#include <QDebug>
#include <QFile>

class AdfsImage
{
public:
    AdfsImage();

    bool open(QString filename);
    void close();
    void readFreeSpaceMap();
    void readDirectory(qint64 sector);

private:
    DiscImage *adfsDiscImage;

};

#endif // ADFSIMAGE_H
