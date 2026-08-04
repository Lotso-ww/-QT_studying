QT += core testlib
CONFIG += console c++11 testcase
CONFIG -= app_bundle
TEMPLATE = app
TARGET = tst_rfidlogdispatcher

INCLUDEPATH += $$PWD/../..

SOURCES += \
    tst_rfidlogdispatcher.cpp \
    ../../rfidlogger.cpp \
    ../../rfidlogdispatcher.cpp

HEADERS += \
    ../../rfidlogger.h \
    ../../rfidlogdispatcher.h
