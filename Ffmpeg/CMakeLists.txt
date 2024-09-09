cmake_minimum_required(VERSION 3.5)

project(FFmpeg_camera_output VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

file(GLOB_RECURSE ALL_SOURCES "./*.cpp", "./*.c")
file(GLOB_RECURSE ALL_HEADERS "./*.hpp", "./*.h")
file(GLOB_RECURSE ALL_UIS "./*.ui")

set(FFMPEG_PREFIX_PATH "C:/ffmpeg")

set(OpenCV_DIR "D:/Program/OpenCV/opencv/build/x64/vc15/lib")

# 查找并加载OpenCV库
find_package(OpenCV REQUIRED)
# 打印OpenCV相关信息以调试
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "OpenCV include path: ${OpenCV_INCLUDE_DIRS}")
# 执行上一行后会自动添加变量 OpenCV_DIR,OpenCV_INCLUDE_DIRS,OpenCV_LIBS

#指定头文件

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${FFMPEG_PREFIX_PATH}/include
)

link_directories(
    ${OpenCV_LIBS}
    ${FFMPEG_PREFIX_PATH}/lib
)

add_executable(FFmpeg_camera_output
    ${ALL_SOURCES}
    ${ALL_HEADERS}
    ${ALL_UIS}
    )

# 链接FFmpeg库
target_link_libraries(FFmpeg_camera_output PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${OpenCV_LIBS} #opencv
    #ffmpeg
    avcodec
    avformat
    avutil
    swscale
    swresample
    )

set_target_properties(FFmpeg_camera_output PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FFmpeg_camera_output)
endif()
