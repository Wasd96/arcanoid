#-------------------------------------------------
#
# Project created by QtCreator 2015-07-01T15:58:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = arcanoid
TEMPLATE = app

SOURCES += main.cpp\
        widget.cpp\
        brick.cpp\
        level.cpp \
    ball.cpp

HEADERS  += widget.h\
        brick.h\
        level.h \
    ball.h

FORMS    += widget.ui

OTHER_FILES += update.log

CONFIG += mobility
MOBILITY =

RESOURCES += \
    resource.qrc
