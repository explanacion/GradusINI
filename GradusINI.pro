#-------------------------------------------------
#
# Project created by QtCreator 2016-11-29T21:42:46
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GradusINI
TEMPLATE = app

# QMAKE_LFLAGS_RELEASE += -static -static-libgcc

SOURCES += main.cpp\
        gradus.cpp

HEADERS  += gradus.h

FORMS    += gradus.ui

RESOURCES += \
    myresources.qrc


LIBS += -L"C:/OpenSSL-Win64/lib" -lssleay32 -lssleay32
INCLUDEPATH += "C:/OpenSSL-Win64/include/openssl"

