#-------------------------------------------------
#
# Project created by QtCreator 2018-04-03T18:03:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenAcornExplorer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    adfsimage.cpp \
    discimage.cpp \
    adfsfreespacemap.cpp \
    adfsdirectory.cpp \
    adfsdirectorymodel.cpp \
    adfsdirectoryentry.cpp

HEADERS += \
        mainwindow.h \
    adfsimage.h \
    discimage.h \
    adfsfreespacemap.h \
    adfsdirectory.h \
    adfsdirectorymodel.h \
    adfsdirectoryentry.h

FORMS += \
        mainwindow.ui
