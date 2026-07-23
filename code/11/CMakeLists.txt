cmake_minimum_required(VERSION 3.5)

project(11 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
  add_library(11 SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
  )
else()
  add_executable(11
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
  )

  # ==================== 新增：IDS Peak SDK 配置开始 ====================
  if(WIN32)
      # 👇 请修改为你电脑上 IDS Peak SDK 的实际安装路径（统一用正斜杠 /）
      set(IDS_PEAK_SDK_ROOT "C:/Program Files/IDS/IDS peak/sdk")

      # 1. 添加头文件搜索路径（指向 include 根目录，匹配 #include <peak/peak.hpp>）
      target_include_directories(11 PRIVATE "${IDS_PEAK_SDK_ROOT}/include")

      # 2. 链接 SDK 静态导入库（64位用 x64，32位请改为 x86）
      target_link_libraries(11 PRIVATE "${IDS_PEAK_SDK_ROOT}/lib/x64/peak.lib")
  endif()
  # ==================== 新增：IDS Peak SDK 配置结束 ====================

endif()

target_link_libraries(11 PRIVATE Qt5::Widgets)
