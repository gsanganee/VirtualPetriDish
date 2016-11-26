#-------------------------------------------------
#
# Project created by QtCreator 2016-10-20T18:22:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CustomGraphicsItem
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    node.cpp \
    element.cpp \
    gmsh.cpp \
    database.cpp \
    simulation.cpp

HEADERS  += mainwindow.h \
    node.h \
    element.h \
    gmsh.h \
    database.h \
    simulation.h \
    myclass.h

FORMS    += mainwindow.ui
