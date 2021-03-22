#-------------------------------------------------
#
# Project created by QtCreator 2019-04-08T16:22:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 2019.06

TARGET = FSR
TEMPLATE = app

SOURCES += main.cpp\
        core.cpp \
        dialog.cpp\
        overlay.cpp \
        parser.cpp

HEADERS  += dialog.h\
            core.h \
            enums.h \
            overlay.h \
            parser.h

FORMS    += dialog.ui

RC_ICONS = icon.ico

DISTFILES += \
    icon.ico

