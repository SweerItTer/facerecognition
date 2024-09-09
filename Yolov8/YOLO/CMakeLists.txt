cmake_minimum_required(VERSION 3.5)

project(yolo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(OpenCV_DIR "D:/Program/OpenCV/opencv/build/x64/vc15/lib")
set(onnxDir "D:/Program/onnxruntime-win-x64-1.10.0/")

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    yolo.cpp
    mainwindow.h
    yolo.h
    mainwindow.ui
)

find_package(OpenCV REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${onnxDir}/include)

link_directories(
    ${onnxDir}/lib)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(yolo_test
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(yolo_test SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(yolo_test
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(yolo_test PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${OpenCV_LIBS}
    onnxruntime)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(yolo_test)
endif()
