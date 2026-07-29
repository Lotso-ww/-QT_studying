cmake_minimum_required(VERSION 3.19)
project(CCD_D LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql)

qt_standard_project_setup()

qt_add_executable(CCD_D
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    camerathread.h
    camerathread.cpp
    dbmanager.h
    dbmanager.cpp
)
include_directories(
    "C:/Program Files/IDS/ids_peak/generic_sdk/api/include"
    "C:/Program Files/IDS/ids_peak/comfort_sdk/api/include"
    "C:/Program Files/IDS/ids_peak/common/include"
    "C:/Program Files/IDS/ids_peak/generic_sdk/ipl/include"
)

target_link_libraries(CCD_D
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql
    "C:/Program Files/IDS/ids_peak/comfort_sdk/api/lib/x86_64/ids_peak_comfort_c.lib"
    "C:/Program Files/IDS/ids_peak/generic_sdk/api/lib/x86_64/ids_peak.lib"
    "C:/Program Files/IDS/ids_peak/generic_sdk/ipl/lib/x86_64/ids_peak_ipl.lib"
)

install(TARGETS CCD_D
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET CCD_D
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
