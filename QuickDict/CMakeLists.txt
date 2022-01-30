set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wno-unused-parameter -Wfatal-errors")
endif()

set(SOURCES
    main.cpp
    service.cpp
    service.h
    monitorservice.cpp
    monitorservice.h
    dictservice.cpp
    dictservice.h
    dictindex.h
    localdict.cpp
    localdict.h
    mobidict.cpp
    mobidict.h
    mdxdict.cpp
    mdxdict.h
    clipboardmonitor.cpp
    clipboardmonitor.h
    configcenter.cpp
    configcenter.h
    quickdict.cpp
    quickdict.h
    utils.cpp
    utils.h
    qml.qrc
    res.qrc
    ${TS_FILES}
)

set(TS_FILES QuickDict_zh_CN.ts)

include_directories(${PROJECT_SOURCE_DIR}/third_party)

set(QT_MIN_VERSION 5.15.2)
find_package(QT NAMES Qt5 REQUIRED Widgets Quick LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} ${QT_MIN_VERSION} REQUIRED Widgets Quick LinguistTools)
set(LIBS Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Quick mobi mdx)

set(TESSERACT_MIN_VERSION 4.1.1)
set(LEPTONICA_MIN_VERSION 1.81.1)
option(ENABLE_TESSERACT "Enable Tesseract" ON)
if(ENABLE_TESSERACT)
    if(UNIX)
        find_package(PkgConfig REQUIRED)
        pkg_search_module(Tesseract ${TESSERACT_MIN_VERSION} REQUIRED tesseract)
        pkg_search_module(Leptonica ${LEPTONICA_MIN_VERSION} REQUIRED lept)
    else()
        find_package(Tesseract ${TESSERACT_MIN_VERSION} REQUIRED tesseract)
        find_package(Leptonica ${LEPTONICA_MIN_VERSION} REQUIRED lept)
    endif()
    add_compile_definitions(ENABLE_TESSERACT)
    list(APPEND SOURCES mouseovermonitor.h mouseovermonitor.cpp ocrengine.h ocrengine.cpp ocrworker.h ocrworker.cpp)
    list(APPEND LIBS ${Tesseract_LIBRARIES} ${Leptonica_LIBRARIES})
endif()

set(OPENCV_MIN_VERSION 4.5.4)
option(ENABLE_OPENCV "Enable OpenCV" ON)
if(ENABLE_OPENCV)
    find_package(OpenCV ${OPENCV_MIN_VERSION} REQUIRED core imgcodecs imgproc)
    add_compile_definitions(ENABLE_OPENCV)
    list(APPEND SOURCES qimagecvmat.h qimagecvmat.cpp)
    list(APPEND LIBS ${OpenCV_LIBRARIES})
endif()

set(OPENCC_MIN_VERSION 1.1.2)
option(ENABLE_OPENCC "Enable OpenCC" ON)
if(ENABLE_OPENCC)
    if(UNIX)
        find_package(PkgConfig REQUIRED)
        pkg_search_module(OpenCC ${OPENCC_MIN_VERSION} REQUIRED opencc)
    else()
        find_package(OpenCC ${OPENCC_MIN_VERSION} REQUIRED opencc)
    endif()
    add_compile_definitions(ENABLE_OPENCC)
    list(APPEND LIBS ${OpenCC_LIBRARIES})
endif()

set(HUNSPELL_MIN_VERSION 1.7.0)
option(ENABLE_HUNSPELL "Enable Hunspell" ON)
if(ENABLE_HUNSPELL)
    if(UNIX)
        find_package(PkgConfig REQUIRED)
        pkg_search_module(Hunspell ${HUNSPELL_MIN_VERSION} REQUIRED hunspell)
    else()
        find_package(Hunspell ${HUNSPELL_MIN_VERSION} REQUIRED hunspell)
    endif()
    add_compile_definitions(ENABLE_HUNSPELL)
    list(APPEND LIBS ${Hunspell_LIBRARIES})
endif()

set(UNAC_MIN_VERSION 1.8.0)
option(ENABLE_UNAC "Enable unac" ON)
if(ENABLE_UNAC)
    add_compile_definitions(ENABLE_UNAC)
    set(Unac_LIBRARIES unac)
    list(APPEND LIBS ${Unac_LIBRARIES})
endif()

set(QHOTKEY_MIN_VERSION 1.5.0)
option(ENABLE_QHOTKEY "Enable QHotkey" ON)
if(ENABLE_QHOTKEY)
    add_compile_definitions(ENABLE_QHOTKEY)
    set(QHotkey_LIBRARIES qhotkey)
    list(APPEND SOURCES hotkey.h hotkey.cpp)
    list(APPEND LIBS ${QHotkey_LIBRARIES})
endif()

set(KF5_MIN_VERSION 5.82.0)
option(ENABLE_KWIN_BLUR "Enable KWin to blur the background" OFF)
if(ENABLE_KWIN_BLUR)
    find_package(ECM ${KF5_MIN_VERSION})
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
    find_package(KF5 ${KF5_MIN_VERSION} REQUIRED WindowSystem)
    add_compile_definitions(ENABLE_KWIN_BLUR)
    list(APPEND LIBS KF5::WindowSystem)
endif()

option(STANDALONE_BUILD "Bundle data within software" OFF)
if(STANDALONE_BUILD)
    add_compile_definitions(STANDALONE_BUILD)
endif()

if(WIN32)
    # Create Win32 application
    set(CMAKE_WIN32_EXECUTABLE ON)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QuickDict
        MANUAL_FINALIZATION
        ${SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET QuickDict APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(QuickDict SHARED
            ${SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(QuickDict
            ${SOURCES}
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
