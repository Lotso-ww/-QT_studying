#QT       += core gui

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#CONFIG += c++11

## The following define makes your compiler emit warnings if you use
## any Qt feature that has been marked deprecated (the exact warnings
## depend on your compiler). Please consult the documentation of the
## deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

## You can also make your code fail to compile if it uses deprecated APIs.
## In order to do so, uncomment the following line.
## You can also select to disable deprecated APIs only up to a certain version of Qt.
##DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#SOURCES += \
#    main.cpp \
#    mainwindow.cpp

#HEADERS += \
#    mainwindow.h

#FORMS += \
#    mainwindow.ui

## Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

QT       += core gui widgets

TARGET = IdsCameraDemo
TEMPLATE = app
CONFIG += c++17

win32 {
    # IDS peak 安装根目录
    IDS_PEAK_ROOT = "C:/Program Files/IDS/ids_peak"

    # ========== 1. 公共基础模块 peak_common（必须加，否则嵌套头文件找不到）==========
    INCLUDEPATH += $$IDS_PEAK_ROOT/common/include

    # ========== 2. 核心 peak API ==========
    INCLUDEPATH += $$IDS_PEAK_ROOT/generic_sdk/api/include
    LIBS += -L$$IDS_PEAK_ROOT/generic_sdk/api/lib/win64
#    LIBS += -lpeak

#    # ========== 3. peak_ipl 图像处理库 ==========
#    INCLUDEPATH += $$IDS_PEAK_ROOT/generic_sdk/ipl/include
#    LIBS += -L$$IDS_PEAK_ROOT/generic_sdk/ipl/lib/win64
#    LIBS += -lpeak_ipl
}

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    # cameroids.cpp

HEADERS += \
    mainwindow.h \
   #  cameroids.h
