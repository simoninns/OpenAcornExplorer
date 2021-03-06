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

//AdfsDirectoryModel::AdfsDirectoryModel(const QStringList &headers, const QString &data, QObject *parent)
AdfsDirectoryModel::AdfsDirectoryModel(DiscImage discImage, QObject *parent)

    : QAbstractItemModel(parent)
{
    // Define the root item
    QVector<QVariant> rootData;
    rootData << "Filename" <<
                "Attr" <<
                "Seq" <<
                "Load" <<
                "Exec" <<
                "Size" <<
                "Sector";

    rootItem = new AdfsDirectoryItem(rootData);

    // Initialise the directory data from the disc image
    initialiseAdfsRootDirectory(&discImage, rootItem);
}

AdfsDirectoryModel::~AdfsDirectoryModel()
{
    delete rootItem;
}

int AdfsDirectoryModel::columnCount(const QModelIndex & /* parent */) const
{
    return rootItem->columnCount();
}

QVariant AdfsDirectoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    AdfsDirectoryItem *item = getItem(index);

    return item->data(index.column());
}

Qt::ItemFlags AdfsDirectoryModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

AdfsDirectoryItem *AdfsDirectoryModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        AdfsDirectoryItem *item = static_cast<AdfsDirectoryItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

QVariant AdfsDirectoryModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex AdfsDirectoryModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    AdfsDirectoryItem *parentItem = getItem(parent);

    AdfsDirectoryItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

bool AdfsDirectoryModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool AdfsDirectoryModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    AdfsDirectoryItem *parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();

    return success;
}

QModelIndex AdfsDirectoryModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    AdfsDirectoryItem *childItem = getItem(index);
    AdfsDirectoryItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool AdfsDirectoryModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = rootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (rootItem->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool AdfsDirectoryModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    AdfsDirectoryItem *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

int AdfsDirectoryModel::rowCount(const QModelIndex &parent) const
{
    AdfsDirectoryItem *parentItem = getItem(parent);

    return parentItem->childCount();
}

bool AdfsDirectoryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    AdfsDirectoryItem *item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index);

    return result;
}

bool AdfsDirectoryModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = rootItem->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

// Initialise the ADFS directory model from the root directory
void AdfsDirectoryModel::initialiseAdfsRootDirectory(DiscImage *discImage, AdfsDirectoryItem *parent)
{
    // Populate the root directory (sector 2)
    populateDirectory(discImage, parent, 2);
}

// Read a directory record and populate the ADFS directory model recursively
void AdfsDirectoryModel::populateDirectory(DiscImage *discImage, AdfsDirectoryItem *parent, qint64 directorySector)
{
    // A directory record is always 5 sectors in length
    QByteArray directoryData;
    directoryData.resize(5 * discImage->getSectorSize());

    // Read 5 sectors of directory data
    directoryData = discImage->readSector(directorySector, 5);

    // Create the directory object (test only!)
    AdfsDirectory adfsDirectory;

    // Put the directory data into the object
    if (adfsDirectory.setDirectory(directoryData)) {
        // Add a child to the parent to contain the directory
        parent->insertChildren(parent->childCount(), 1, rootItem->columnCount());

        // Add the entry detail columns to the parent
        qDebug() << "AdfsImage::readDirectory(): Reading directory data for directory" << adfsDirectory.getDirectoryName();
        parent->child(parent->childCount() - 1)->setData(0, adfsDirectory.getDirectoryName());
        parent->child(parent->childCount() - 1)->setData(2, adfsDirectory.getMasterSequenceNumber());

        // Make the new child the parent
        QList<AdfsDirectoryItem*> dir;
        dir << parent->child(parent->childCount() - 1);
        AdfsDirectoryItem *parent = dir.last();

        // Now add a child to this parent for every entry in the directory
        qint64 entry = 0;
        // An empty entry name indicates the end of the directory
        while (!adfsDirectory.getEntryName(entry).isEmpty() && entry <= 47) {
            // Is this a file or a directory entry?
            if (adfsDirectory.isEntryDirectory(entry)) {
                // Entry is a directory

                // Populate the sub-directory
                populateDirectory(discImage, parent, adfsDirectory.getEntryStartSector(entry));
            } else {
                // Entry is a file - populate the details
                qDebug() << "Entry" << adfsDirectory.getEntryName(entry) << "is a file";

                // Add a child to contain the file details
                parent->insertChildren(parent->childCount(), 1, rootItem->columnCount());

                parent->child(parent->childCount() - 1)->setData(0, adfsDirectory.getEntryName(entry));
                parent->child(parent->childCount() - 1)->setData(2, adfsDirectory.getEntrySequenceNumber(entry));
                parent->child(parent->childCount() - 1)->setData(3, adfsDirectory.getEntryLoadAddress(entry));
                parent->child(parent->childCount() - 1)->setData(4, adfsDirectory.getEntryExecutionAddress(entry));
                parent->child(parent->childCount() - 1)->setData(5, adfsDirectory.getEntryLength(entry));
                parent->child(parent->childCount() - 1)->setData(6, adfsDirectory.getEntryStartSector(entry));
            }

            // Next
            entry++;
        }
    }
}
