cmake_minimum_required(VERSION 3.16)

project(hap_installer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES hap_installer_zh_CN.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        ${TS_FILES}
)

IF (WIN32)
	# Windows
ELSEIF (APPLE)
	# Apple
    set(MACOSX_BUNDLE true)
    set(MACOSX_BUNDLE_BUNDLE_NAME "Hap安装器")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.devzeng.hap_installer")
    set(MACOSX_BUNDLE_ICON_FILE "AppIcon.icns")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "1.0.0.1")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")

    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/AppIcon.icns" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    set(QUAZIP_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/quazip/mac/include")
    set(QUAZIP_LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/quazip/mac/libs")
    set(QUAZIP_LIBS libquazip.1.dylib)
    include_directories(${QUAZIP_INC_DIR})
    link_directories(${QUAZIP_LIBS_DIR})
ENDIF ()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/hap_installer.app/Contents/MacOS/hdc_cli)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/hdc_cli DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/hap_installer.app/Contents/MacOS/)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(hap_installer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET hap_installer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(hap_installer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        if(APPLE)
            add_executable(hap_installer MACOSX_BUNDLE ${PROJECT_SOURCES})
        else()
            add_executable(hap_installer ${PROJECT_SOURCES})
        endif()
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(hap_installer Qt${QT_VERSION_MAJOR}::Widgets ${QUAZIP_LIBS})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.devzeng.hap_installer)
endif()
set_target_properties(hap_installer PROPERTIES
    ${BUNDLE_ID_OPTION}
    RESOURCE "./AppIcon.icns"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS hap_installer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(hap_installer)
endif()
