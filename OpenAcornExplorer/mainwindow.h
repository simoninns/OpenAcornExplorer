/************************************************************************

    mainwindow.h

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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QModelIndex>

#include "aboutdialog.h"
#include "discimage.h"
#include "adfsdirectorymodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    // Model slots
    void updateActions();

private slots:
    // Model slots
    void insertChild();
    bool insertColumn();
    void insertRow();
    bool removeColumn();
    void removeRow();

    // Menu bar trigger methods
    void on_actionAbout_OpenAcornExplorer_triggered();
    void on_actionExit_triggered();
    void on_actionOpen_triggered();

private:
    Ui::MainWindow *ui;
    AboutDialog *aboutDialog;
    QLabel *status;
    QString discImageFilename;

    DiscImage *discImage;
    AdfsDirectoryModel *adfsDirectoryModel;
};

#endif // MAINWINDOW_H
