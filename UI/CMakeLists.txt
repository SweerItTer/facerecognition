cmake_minimum_required(VERSION 3.5)

project(MyQtApp VERSION 0.1 LANGUAGES CXX)

# set(CMAKE_PREFIX_PATH "D:/Program/Qt5.12/5.12.12/msvc2017_64")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts PrintSupport Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts PrintSupport Sql)

# OpenCV 设置

set(OpenCV_DIR "D:/Program/OpenCV/opencv/build/x64/vc14/lib")

find_package(OpenCV REQUIRED)

# 项目源文件
set(SOURCES
    mainwindow.cpp
    qcustomplot.cpp
)

# 项目头文件
set(HEADERS
    mainwindow.h
    qcustomplot.h
)

# UI 文件
set(FORMS
    mainwindow.ui
)

# 资源文件
set(RESOURCES
    Resources/Resources.qrc
)

# 将源文件、头文件和 UI 文件添加为目标
add_executable(MyQtApp
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
    ${RESOURCES}
)

# 链接 Qt 库和 OpenCV 库
target_link_libraries(MyQtApp PRIVATE 
    ${OpenCV_LIBS}
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Qt${QT_VERSION_MAJOR}::Sql
    )

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MyQtApp)
endif()
