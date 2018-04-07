/************************************************************************

    adfsdirectorymodel.cpp

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

#include "adfsdirectorymodel.h"

// Construct the directory entry model
AdfsDirectoryModel::AdfsDirectoryModel(const QString &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    qDebug() << "DirectoryEntryModel::DirectoryEntryModel(): Creating object";
    // Set up the root
    QList<QVariant> rootData;
    rootData << "Title" << "Summary";
    rootItem = new AdfsDirectoryEntry(rootData);
    setupModelData(data.split(QString("\n")), rootItem);
}

// Destruct the directory entry model
AdfsDirectoryModel::~AdfsDirectoryModel()
{
    qDebug() << "DirectoryEntryModel::~DirectoryEntryModel(): Deleting object";
    delete rootItem;
}

// Fetch the index for an entry
QModelIndex AdfsDirectoryModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    AdfsDirectoryEntry *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<AdfsDirectoryEntry*>(parent.internalPointer());

    AdfsDirectoryEntry *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

// Fetch the parent for an index
QModelIndex AdfsDirectoryModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    AdfsDirectoryEntry *childItem = static_cast<AdfsDirectoryEntry*>(index.internalPointer());
    AdfsDirectoryEntry *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

// Count the number of rows
int AdfsDirectoryModel::rowCount(const QModelIndex &parent) const
{
    AdfsDirectoryEntry *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<AdfsDirectoryEntry*>(parent.internalPointer());

    return parentItem->childCount();
}

// Count the number of columns
int AdfsDirectoryModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<AdfsDirectoryEntry*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

// Get the data for an index
QVariant AdfsDirectoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    AdfsDirectoryEntry *item = static_cast<AdfsDirectoryEntry*>(index.internalPointer());

    return item->data(index.column());
}

// Get the flags for an index
Qt::ItemFlags AdfsDirectoryModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

// Get the header data for the model
QVariant AdfsDirectoryModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

// Initialise the model's data
void AdfsDirectoryModel::setupModelData(const QStringList &lines, AdfsDirectoryEntry *parent)
{
    QList<AdfsDirectoryEntry*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

    while (number < lines.count()) {
        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].at(position) != ' ')
                break;
            position++;
        }

        QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
            qDebug() << "Reading column data:" << lineData;
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
            QList<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column) {
                columnData << columnStrings[column];
                qDebug() << columnStrings[column];
            }

            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            parents.last()->appendChild(new AdfsDirectoryEntry(columnData, parents.last()));
            qDebug() << "Appended child to model";
        } else qDebug() << "Reading empty line...";

        ++number;
    }
}
