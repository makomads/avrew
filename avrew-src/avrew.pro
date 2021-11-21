#-------------------------------------------------
#
# Project created by QtCreator 2019-10-21T12:51:09
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = avrew
TEMPLATE = app
win32:RC_FILE = winresource.rc


SOURCES +=\
        mainwindow.cpp \
    bridgeio.cpp \
    comthread.cpp \
    configdialog.cpp \
    console.cpp \
    deviceinfo.cpp \
    functionbutton.cpp \
    intelhex.cpp \
    simplecrypt.cpp \
    versiondialog.cpp

HEADERS  += mainwindow.h \
    bridgeio.h \
    comthread.h \
    configdialog.h \
    console.h \
    deviceinfo.h \
    functionbutton.h \
    intelhex.h \
    simplecrypt.h \
    version.h \
    versiondialog.h

FORMS    += mainwindow.ui \
    configdialog.ui \
    functionbutton.ui \
    versiondialog.ui

RESOURCES += \
    resource.qrc

TRANSLATIONS+=  \
    translation_ja.ts

DISTFILES += \
    device.xml \
    translation_ja.ts \
    winresource.rc
