cmake_minimum_required(VERSION 3.5)

project(CCDController LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

if(ANDROID)
  add_library(CCDController SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
  )
else()
  add_executable(CCDController
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
#    camerathread.cpp
#    camerathread.h
  )
endif()

include_directories(
    "C:/Program Files/IDS/ids_peak/generic_sdk/api/include"
    "C:/Program Files/IDS/ids_peak/comfort_sdk/api/include"
    "C:/Program Files/IDS/ids_peak/common/include"
)

target_link_libraries(CCDController PRIVATE Qt5::Widgets
    "C:/Program Files/IDS/ids_peak/generic_sdk/api/lib/x86_64/ids_peak.lib"
    "C:/Program Files/IDS/ids_peak/comfort_sdk/api/lib/x86_64/ids_peak_comfort_c.lib"
)
