QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS



# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CAEDevice_HF.cpp \
    gfunction.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    CAEDevice_HF.h \
    c++_lib/inc/device_driver/rfidlib_ANM103R.h \
    c++_lib/inc/device_driver/rfidlib_ANMF102U.h \
    c++_lib/inc/device_driver/rfidlib_ANMR113R.h \
    c++_lib/inc/device_driver/rfidlib_ANRD100.h \
    c++_lib/inc/device_driver/rfidlib_ANRD120M.h \
    c++_lib/inc/device_driver/rfidlib_ANRD122.h \
    c++_lib/inc/device_driver/rfidlib_ANRD131.h \
    c++_lib/inc/device_driver/rfidlib_ANRD201.h \
    c++_lib/inc/device_driver/rfidlib_ANRD242.h \
    c++_lib/inc/device_driver/rfidlib_ANRD501.h \
    c++_lib/inc/device_driver/rfidlib_ANRD503.h \
    c++_lib/inc/device_driver/rfidlib_ANRD5112.h \
    c++_lib/inc/device_driver/rfidlib_ANRD543.h \
    c++_lib/inc/device_driver/rfidlib_ANSSR100.h \
    c++_lib/inc/device_driver/rfidlib_ANWE818.h \
    c++_lib/inc/device_driver/rfidlib_G101.h \
    c++_lib/inc/device_driver/rfidlib_G302.h \
    c++_lib/inc/device_driver/rfidlib_anmf103R.h \
    c++_lib/inc/device_driver/rfidlib_drv_G310.h \
    c++_lib/inc/device_driver/rfidlib_drv_LSGControlCenter.h \
    c++_lib/inc/device_driver/rfidlib_drv_M200.h \
    c++_lib/inc/device_driver/rfidlib_drv_M201.h \
    c++_lib/inc/device_driver/rfidlib_drv_M50.h \
    c++_lib/inc/device_driver/rfidlib_drv_M60.h \
    c++_lib/inc/device_driver/rfidlib_drv_MGxxx.h \
    c++_lib/inc/device_driver/rfidlib_drv_RD5100.h \
    c++_lib/inc/device_driver/rfidlib_drv_RD5100F.h \
    c++_lib/inc/device_driver/rfidlib_drv_RL8000.h \
    c++_lib/inc/device_driver/rfidlib_drv_RPAN.h \
    c++_lib/inc/device_driver/rfidlib_drv_RPAN_UHF.h \
    c++_lib/inc/device_driver/rfidlib_drv_UM200.h \
    c++_lib/inc/device_driver/rfidlib_drv_led_lock_light.h \
    c++_lib/inc/rfidlib.h \
    c++_lib/inc/rfidlib_Gxxx.h \
    c++_lib/inc/rfidlib_LSGate.h \
    c++_lib/inc/rfidlib_aip_NFCForumType1.h \
    c++_lib/inc/rfidlib_aip_SonyFelica.h \
    c++_lib/inc/rfidlib_aip_iso14443A.h \
    c++_lib/inc/rfidlib_aip_iso14443b.h \
    c++_lib/inc/rfidlib_aip_iso15693.h \
    c++_lib/inc/rfidlib_aip_iso18000p3m3.h \
    c++_lib/inc/rfidlib_aip_iso18000p6c.h \
    c++_lib/inc/rfidlib_aip_st_iso14443b.h \
    c++_lib/inc/rfidlib_nfc.h \
    c++_lib/inc/rfidlib_picc_MifareDesfire.h \
    c++_lib/inc/rfidlib_picc_MifarePlus.h \
    c++_lib/inc/rfidlib_reader.h \
    gfunction.h \
    mainwindow.h \
    tag_hf.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/lib_win32_debug/ -lrfidlib_reader

INCLUDEPATH += $$PWD/lib_win32_debug
DEPENDPATH += $$PWD/lib_win32_debug

win32: LIBS += -L$$PWD/lib_win32_debug/ -lrfidlib_aip_iso15693

INCLUDEPATH += $$PWD/lib_win32_debug
DEPENDPATH += $$PWD/lib_win32_debug

win32: LIBS += -L$$PWD/lib_win32_debug/device_driver/ -lrfidlib_drv_RL8000

INCLUDEPATH += $$PWD/lib_win32_debug/device_driver
DEPENDPATH += $$PWD/lib_win32_debug/device_driver

win32: LIBS += -L$$PWD/c++_lib/lib_win32_debug/ -lrfidlib_aip_iso14443A

INCLUDEPATH += $$PWD/c++_lib/lib_win32_debug
DEPENDPATH += $$PWD/c++_lib/lib_win32_debug


QMAKE_LFLAGS = /NOLOGO /DYNAMICBASE /NXCOMPAT /DEBUG /SUBSYSTEM:WINDOWS /MANIFEST:NO
QMAKE_MANIFEST = 
CONFIG -= embed_manifest_dll embed_manifest_exe
QMAKE_LFLAGS_EXE =
