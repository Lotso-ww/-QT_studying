QT += core testlib

CONFIG += c++11 testcase console
CONFIG -= app_bundle

TEMPLATE = app
TARGET = tst_rfidlogger

INCLUDEPATH += ../../

SOURCES += \
    tst_rfidlogger.cpp \
    ../../rfidlogdispatcher.cpp \
    ../../rfidlogger.cpp

HEADERS += \
    ../../rfidlogdispatcher.h \
    ../../rfidlogger.h

# Keep the test link settings consistent with the main MSVC project.
QMAKE_LFLAGS = /NOLOGO /DYNAMICBASE /NXCOMPAT /DEBUG /SUBSYSTEM:CONSOLE /MANIFEST:NO
QMAKE_MANIFEST =
CONFIG -= embed_manifest_dll embed_manifest_exe
QMAKE_LFLAGS_EXE =
