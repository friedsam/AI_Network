#-------------------------------------------------
#
# Project created by QtCreator 2011-12-02T20:33:56
#
#-------------------------------------------------

include( $${PWD}/configuration.pri )

QT       += core gui # qwt

TARGET = AiNetwork


SOURCES += main.cpp\
        mainwindow.cpp \
    aicontroller.cpp \
    aievolution.cpp \
    aigenome.cpp \
    aineuralnet.cpp \
    aitrainingset.cpp

HEADERS  += mainwindow.h \
    aicontroller.h \
    aievolution.h \
    aigenome.h \
    aineuralnet.h \
    aitrainingset.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    configuration.pri \
    qwtconfig.pri \
    qwtbuild.pri





















