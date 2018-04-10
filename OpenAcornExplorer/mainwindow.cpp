/************************************************************************

    mainwindow.cpp

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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Add a label to the status bar for showing the status
    status = new QLabel;
    ui->statusBar->addWidget(status);

    // Set the status
    status->setText(tr("No disc image loaded"));



    // Test code for model
//    QStringList headers;
//    headers << tr("Heading 1") << tr("Heading 2");

//    //QFile file(":/default.txt");
//    //file.open(QIODevice::ReadOnly);
//    AdfsDirectoryModel *model = new AdfsDirectoryModel(headers);
//    //file.close();

//    ui->treeView->setModel(model);
//    for (int column = 0; column < model->columnCount(); ++column)
//        ui->treeView->resizeColumnToContents(column);

//    connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
//            this, &MainWindow::updateActions);

//    connect(ui->menuTools, &QMenu::aboutToShow, this, &MainWindow::updateActions);
//    connect(ui->actionInsert_Row, &QAction::triggered, this, &MainWindow::insertRow);
//    connect(ui->actionInsert_Column, &QAction::triggered, this, &MainWindow::insertColumn);
//    connect(ui->actionRemove_Row, &QAction::triggered, this, &MainWindow::removeRow);
//    connect(ui->actionRemove_Column, &QAction::triggered, this, &MainWindow::removeColumn);
//    connect(ui->actionInsert_Child, &QAction::triggered, this, &MainWindow::insertChild);

    //updateActions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Menu bar trigger methods -------------------------------------------------------------------------------------------

// User triggered Menu->Help->About
void MainWindow::on_actionAbout_OpenAcornExplorer_triggered()
{
    aboutDialog = new AboutDialog(this);
    aboutDialog->show();
}

// User triggered Menu->File->Exit
void MainWindow::on_actionExit_triggered()
{
    // Quit the application
    qApp->quit();
}

// User triggered Menu->File->Open...
void MainWindow::on_actionOpen_triggered()
{
    // Display the open file dialogue
    discImageFilename = QFileDialog::getOpenFileName(this,
            tr("Open Acorn disc image"),
            //QDir::homePath(),
            "D:\\simon\\Documents\\GitHub\\OpenAcornExplorer\\ADFS Test images",
            tr("ADFS images (*.adl *.adf *.dat);;DFS images (*.ssd *.dsd *.img);;All files (*.*)"));

    // Check for empty response
    if (discImageFilename.isEmpty()) return;

    // Open the disc image file
    discImage = new DiscImage(discImageFilename);

    // Is the disc image valid?
    if (!discImage->isValid()) {
        QMessageBox::information(this, tr("Unable to open disc image"),
            tr("Disc image invalid"));

        // Delete the disc image object
        delete discImage;

        // Exit
        return;
    }

    // Determine the disc image type
    // Not implemented yet - always ADFS L

    // Create the model and update the UI treeview
    adfsDirectoryModel = new AdfsDirectoryModel(*discImage);
    ui->treeView->setModel(adfsDirectoryModel);
    ui->treeView->setColumnWidth(0,200);    // Filename
    ui->treeView->setColumnWidth(1,50);     // Attr
    ui->treeView->setColumnWidth(2,50);     // Seq
    ui->treeView->setColumnWidth(3,50);     // Load
    ui->treeView->setColumnWidth(4,50);     // Exec
    ui->treeView->setColumnWidth(5,50);     // Size
    ui->treeView->setColumnWidth(6,50);     // Sector

    // Update the status bar
    status->setText(tr("Disc image loaded"));
}

// Model methods (Test) -----------------------------------------------------------------------------------------------

void MainWindow::insertChild()
{
    QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    QAbstractItemModel *model = ui->treeView->model();

    if (model->columnCount(index) == 0) {
        if (!model->insertColumn(0, index))
            return;
    }

    if (!model->insertRow(0, index))
        return;

    for (int column = 0; column < model->columnCount(index); ++column) {
        QModelIndex child = model->index(0, column, index);
        model->setData(child, QVariant("[No data]"), Qt::EditRole);
        if (!model->headerData(column, Qt::Horizontal).isValid())
            model->setHeaderData(column, Qt::Horizontal, QVariant("[No header]"), Qt::EditRole);
    }

    ui->treeView->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                            QItemSelectionModel::ClearAndSelect);
    updateActions();
}

bool MainWindow::insertColumn()
{
    QAbstractItemModel *model = ui->treeView->model();
    int column = ui->treeView->selectionModel()->currentIndex().column();

    // Insert a column in the parent item.
    bool changed = model->insertColumn(column + 1);
    if (changed)
        model->setHeaderData(column + 1, Qt::Horizontal, QVariant("[No header]"), Qt::EditRole);

    updateActions();

    return changed;
}

void MainWindow::insertRow()
{
    QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    QAbstractItemModel *model = ui->treeView->model();

    if (!model->insertRow(index.row()+1, index.parent()))
        return;

    updateActions();

    for (int column = 0; column < model->columnCount(index.parent()); ++column) {
        QModelIndex child = model->index(index.row()+1, column, index.parent());
        model->setData(child, QVariant("[No data]"), Qt::EditRole);
    }
}

bool MainWindow::removeColumn()
{
    QAbstractItemModel *model = ui->treeView->model();
    int column = ui->treeView->selectionModel()->currentIndex().column();

    // Insert columns in each child of the parent item.
    bool changed = model->removeColumn(column);

    if (changed)
        updateActions();

    return changed;
}

void MainWindow::removeRow()
{
    QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    QAbstractItemModel *model = ui->treeView->model();
    if (model->removeRow(index.row(), index.parent()))
        updateActions();
}

void MainWindow::updateActions()
{
    bool hasSelection = !ui->treeView->selectionModel()->selection().isEmpty();
    ui->actionRemove_Row->setEnabled(hasSelection);
    ui->actionRemove_Column->setEnabled(hasSelection);

    bool hasCurrent = ui->treeView->selectionModel()->currentIndex().isValid();
    ui->actionInsert_Row->setEnabled(hasCurrent);
    ui->actionInsert_Column->setEnabled(hasCurrent);

    if (hasCurrent) {
        ui->treeView->closePersistentEditor(ui->treeView->selectionModel()->currentIndex());

        int row = ui->treeView->selectionModel()->currentIndex().row();
        int column = ui->treeView->selectionModel()->currentIndex().column();
        if (ui->treeView->selectionModel()->currentIndex().parent().isValid())
            statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        else
            statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));
    }
}
