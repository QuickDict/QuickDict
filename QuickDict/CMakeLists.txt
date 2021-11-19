set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Quick WebEngineWidgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Quick WebEngineWidgets LinguistTools REQUIRED)
find_package(OpenCV REQUIRED)

if(WIN32)
    find_package(Tesseract REQUIRED)
    find_package(Leptonica REQUIRED)
else()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(Tesseract REQUIRED tesseract)
    pkg_search_module(Leptonica REQUIRED lept)
endif()

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Tesseract_INCLUDE_DIRS})
include_directories(${Leptonica_INCLUDE_DIRS})


set(TS_FILES QuickDict_zh_CN.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        qimagecvmat.cpp
        qimagecvmat.h
        monitorinterface.cpp
        monitorinterface.h
        mouseovermonitor.cpp
        mouseovermonitor.h
        clipboardmonitor.cpp
        clipboardmonitor.h
        monitorservice.cpp
        monitorservice.h
        dictinterface.cpp
        dictinterface.h
        dictservice.cpp
        dictservice.h
        shortcutmanager.cpp
        shortcutmanager.h
        ocrengine.cpp
        ocrengine.h
        ocrworker.cpp
        ocrworker.h
        configcenter.cpp
        configcenter.h
        quickdict.cpp
        quickdict.h
        qml.qrc
        res.qrc
        ${TS_FILES}
)

set(LIBS Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::WebEngineWidgets qhotkey ${OpenCV_LIBS} ${Tesseract_LIBRARIES} ${Leptonica_LIBRARIES})

set (KF5_MIN_VERSION "5.82.0")
option(ENABLE_KWIN_BLUR "Enable KWin to blur the background" OFF)
if(ENABLE_KWIN_BLUR)
    find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
    find_package(KF5 ${KF5_MIN_VERSION} OPTIONAL_COMPONENTS WindowSystem QUIET)

    add_compile_definitions(KF5_FOUND)
    list(APPEND LIBS KF5::WindowSystem)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QuickDict
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET QuickDict APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(QuickDict SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(QuickDict
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
endif()

target_compile_definitions(QuickDict
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(QuickDict PRIVATE ${LIBS})
install(TARGETS QuickDict DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")

set_target_properties(QuickDict PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(QuickDict)
    qt_finalize_executable(QuickDict)
endif()
