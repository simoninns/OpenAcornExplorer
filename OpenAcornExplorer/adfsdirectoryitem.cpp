/************************************************************************

    adfsdirectoryitem.cpp

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

#include "adfsdirectoryitem.h"

AdfsDirectoryItem::AdfsDirectoryItem(const QVector<QVariant> &data, AdfsDirectoryItem *parent)
{
    parentItem = parent;
    itemData = data;
}

AdfsDirectoryItem::~AdfsDirectoryItem()
{
    qDeleteAll(childItems);
}

AdfsDirectoryItem *AdfsDirectoryItem::child(int number)
{
    return childItems.value(number);
}

int AdfsDirectoryItem::childCount() const
{
    return childItems.count();
}

int AdfsDirectoryItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<AdfsDirectoryItem*>(this));

    return 0;
}

int AdfsDirectoryItem::columnCount() const
{
    return itemData.count();
}

QVariant AdfsDirectoryItem::data(int column) const
{
    return itemData.value(column);
}

bool AdfsDirectoryItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > childItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        AdfsDirectoryItem *item = new AdfsDirectoryItem(data, this);
        childItems.insert(position, item);
    }

    return true;
}

bool AdfsDirectoryItem::insertColumns(int position, int columns)
{
    if (position < 0 || position > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.insert(position, QVariant());

    foreach (AdfsDirectoryItem *child, childItems)
        child->insertColumns(position, columns);

    return true;
}

AdfsDirectoryItem *AdfsDirectoryItem::parent()
{
    return parentItem;
}

bool AdfsDirectoryItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);

    return true;
}

bool AdfsDirectoryItem::removeColumns(int position, int columns)
{
    if (position < 0 || position + columns > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.remove(position);

    foreach (AdfsDirectoryItem *child, childItems)
        child->removeColumns(position, columns);

    return true;
}

bool AdfsDirectoryItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;
    return true;
}
