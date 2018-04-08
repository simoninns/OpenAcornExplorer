/************************************************************************

    adfsdirectoryitem.h

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

#ifndef ADFSDIRECTORYITEM_H
#define ADFSDIRECTORYITEM_H

#include <QList>
#include <QVariant>
#include <QVector>
#include <QStringList>

class AdfsDirectoryItem
{
public:
    explicit AdfsDirectoryItem(const QVector<QVariant> &data, AdfsDirectoryItem *parent = 0);
    ~AdfsDirectoryItem();

    AdfsDirectoryItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);
    AdfsDirectoryItem *parent();
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool setData(int column, const QVariant &value);

private:
    QList<AdfsDirectoryItem*> childItems;
    QVector<QVariant> itemData;
    AdfsDirectoryItem *parentItem;
};

#endif // ADFSDIRECTORYITEM_H
