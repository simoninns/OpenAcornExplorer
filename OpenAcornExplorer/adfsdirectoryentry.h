/************************************************************************

    adfsdirectoryentry.h

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

#ifndef ADFSDIRECTORYENTRY_H
#define ADFSDIRECTORYENTRY_H

#include <QApplication>
#include <QDebug>

class AdfsDirectoryEntry
{
public:
    explicit AdfsDirectoryEntry(const QList<QVariant> &data, AdfsDirectoryEntry *parentItem = 0);
    ~AdfsDirectoryEntry();

    void appendChild(AdfsDirectoryEntry *child);

    AdfsDirectoryEntry *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    AdfsDirectoryEntry *parentItem();

private:
    QList<AdfsDirectoryEntry*> m_childItems;
    QList<QVariant> m_itemData;
    AdfsDirectoryEntry *m_parentItem;
};

#endif // ADFSDIRECTORYENTRY_H
