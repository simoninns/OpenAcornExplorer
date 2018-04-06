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
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Test button - Create image
void MainWindow::on_testCreateImagePushButton_clicked()
{

}

// Test button - Open image
void MainWindow::on_testOpenImagePushButton_clicked()
{
    // Create an ADFS image object
    AdfsImage adfsImage;

    // Open an image file (ADFS L format 640K image)
    qDebug() << "MainWindow::on_testOpenImagePushButton_clicked(): Opening ADFS image";
    adfsImage.open("D:\\simon\\Documents\\GitHub\\OpenAcornExplorer\\ADFS Test images\\ADFS L 640K.adl");
    //adfsImage.open("D:\\simon\\Documents\\GitHub\\OpenAcornExplorer\\ADFS Test images\\ADFS M 320K.adf");

    // Perform some tests
    qDebug() << "MainWindow::on_testOpenImagePushButton_clicked(): Reading free space map";
    adfsImage.readFreeSpaceMap();

    qDebug() << "MainWindow::on_testOpenImagePushButton_clicked(): Reading root directory";
    adfsImage.readDirectory(2); // Read the root directory (sector 2)

    // Close the image file
    qDebug() << "MainWindow::on_testOpenImagePushButton_clicked(): Closing ADFS image";
    adfsImage.close();

    qDebug() << "MainWindow::on_testOpenImagePushButton_clicked(): Done";
}

// Test button - List directory
void MainWindow::on_testListDirectoryPushButton_clicked()
{

}

// Test button - Close image
void MainWindow::on_testCloseImagePushButton_clicked()
{

}
