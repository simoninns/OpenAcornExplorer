/************************************************************************

    adfsdirectoryentry.cpp

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

#include "adfsdirectoryentry.h"

AdfsDirectoryEntry::AdfsDirectoryEntry(const QList<QVariant> &data, AdfsDirectoryEntry *parent)
{
    m_parentItem = parent;
    m_itemData = data;
}

AdfsDirectoryEntry::~AdfsDirectoryEntry()
{
    qDeleteAll(m_childItems);
}

void AdfsDirectoryEntry::appendChild(AdfsDirectoryEntry *item)
{
    m_childItems.append(item);
}

AdfsDirectoryEntry *AdfsDirectoryEntry::child(int row)
{
    return m_childItems.value(row);
}

int AdfsDirectoryEntry::childCount() const
{
    return m_childItems.count();
}

int AdfsDirectoryEntry::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<AdfsDirectoryEntry*>(this));

    return 0;
}

int AdfsDirectoryEntry::columnCount() const
{
    return m_itemData.count();
}

QVariant AdfsDirectoryEntry::data(int column) const
{
    return m_itemData.value(column);
}

AdfsDirectoryEntry *AdfsDirectoryEntry::parentItem()
{
    return m_parentItem;
}
