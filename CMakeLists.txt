cmake_minimum_required(VERSION 3.5)
project(FaceRecognition VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 查找并添加 Qt5 库
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts PrintSupport Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts PrintSupport Sql)

include_directories(./Sql/hnswlib)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Linux-specific flags
    add_definitions(-g -O0 -ggdb -gdwarf -funwind-tables -rdynamic)
    add_definitions(-Wno-write-strings -Wno-return-type)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Windows-specific flags (adjust for MSVC or MinGW)
    if (MSVC)
        # MSVC-specific options
        add_definitions(/Zi /Od)  # Equivalent to -g -O0
        add_definitions(/W3)      # Warning level 3
        add_definitions(/wd4996)  # Disable specific warning (deprecated functions)
    elseif (MINGW)
        # MinGW-specific options (if you're using GCC under Windows)
        add_definitions(-g -O0)
        add_definitions(-Wno-write-strings -Wno-return-type)
    endif()
endif()

if (MSVC)
    # 忽略警告 C4819
    add_compile_options(/wd4819)
    # 忽略警告 C4267
    add_compile_options(/wd4267)  
endif()

# Set paths to dependencies
## hjj path
set(FFMPEG_PREFIX_PATH "C:/ffmpeg")
set(onnxDir "D:/Program/onnxruntime-win-x64-1.10.0/")
set(OpenCV_DIR "D:/Program/OpenCV/opencv/build/x64/vc15/lib")
set(sqlDir "C:/Program Files/MySQL/MySQL Server 8.0")
## hhl path
# set(OpenCV_DIR "F:/OpenCV/opencv-4.5.1/build/x64/vc15/lib")
# set(onnxDir "F:/onnxruntime-win-x64-1.10.0/")
# set(FFMPEG_PREFIX_PATH "F:/FFmpeg/ffmpeg-6.1.1-full_build-shared")
# set(sqlDir "D:/MySql")

if (sqlDir)
    include_directories(${sqlDir}/include)
    link_directories(${sqlDir}/lib)
endif()
# Find required packages
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# include qt lib (if Qt5_INCLUDE_DIRS donot include)
include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})

# If onnxDir is provided, set include and lib paths
if (onnxDir)
    include_directories(${onnxDir}/include)
    link_directories(${onnxDir}/lib)
else()
    message(FATAL_ERROR "ONNX Runtime directory not provided. Please set onnxDir.")
endif()

# If FFmpeg is found or path is provided, set include and lib paths
if (FFMPEG_PREFIX_PATH)
    include_directories(${FFMPEG_PREFIX_PATH}/include)
    link_directories(${FFMPEG_PREFIX_PATH}/lib)
else()
    message(FATAL_ERROR "FFmpeg directory not provided. Please set FFMPEG_PREFIX_PATH.")
endif()

# Glob the source and header files
file(GLOB_RECURSE Yolo_source "./Yolov8/*.cpp" "./Yolov8/*.c")
file(GLOB_RECURSE Ffmpeg_source "./Ffmpeg/*.cpp" "./Ffmpeg/*.c")
file(GLOB_RECURSE Facenet_source "./Facenet/*.cpp" "./Facenet/*.c")
file(GLOB_RECURSE UI_source "./UI/*.cpp" "./UI/*.c")
file(GLOB_RECURSE Sql_source "./Sql/*.cpp" "./Sql/*.c")

file(GLOB_RECURSE Yolo_header "./Yolov8/*.h" "./Yolov8/*.hpp")
file(GLOB_RECURSE Ffmpeg_header "./Ffmpeg/*.h" "./Ffmpeg/*.hpp")
file(GLOB_RECURSE Facenet_header "./Facenet/*.h" "./Facenet/*.hpp")
file(GLOB_RECURSE UI_header "./UI/*.h" "./UI/*.hpp")
file(GLOB_RECURSE Sql_header "./Sql/*.h" "./Sql/*.hpp")

# UI 文件
set(FORMS
    ./UI/mainwindow.ui
)

# 资源文件
set(RESOURCES
    ./UI/Resources/Resources.qrc
)

# Combine sources and headers
set(PROJECT_SOURCES
    ${Yolo_source}
    ${Ffmpeg_source}
    ${Facenet_source}
    ${UI_source}
    ${Sql_source}

    ${Yolo_header}
    ${Ffmpeg_header}
    ${Facenet_header}
    ${UI_header}
    ${Sql_header}

    main.cpp
    script.cpp
    script.h
    imageprocessor.h
    ${RESOURCES}
    ${FORMS}
    )

# Add executable target
add_executable(FaceRecognition ${PROJECT_SOURCES})

# Link libraries to the target
target_link_libraries(FaceRecognition PRIVATE
    # qt5
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::PrintSupport
    # Qt${QT_VERSION_MAJOR}::Sql
    ${OpenCV_LIBS}
    onnxruntime
    libmysql
    #ffmpeg
    avcodec
    avformat
    avutil
    swscale
    swresample
)

# Optionally, set output directory for the executable
set_target_properties(FaceRecognition PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(TARGETS FaceRecognition DESTINATION bin)
