cmake_minimum_required(VERSION 3.5)
project(FaceRecognition VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set paths to dependencies
# set(OpenCV_DIR "D:/Program/OpenCV/opencv/build/x64/vc15/lib")
set(onnxDir "D:/Program/onnxruntime-win-x64-1.10.0/")
set(FFMPEG_PREFIX_PATH "C:/ffmpeg")

# Find required packages
find_package(OpenCV REQUIRED)

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
file(GLOB_RECURSE Yolo_source "./Yolov8/*.cpp", "./Yolov8/*.c")
file(GLOB_RECURSE Ffmpeg_source "./Ffmpeg/*.cpp", "./Ffmpeg/*.c")
file(GLOB_RECURSE Facenet_source "./Facenet/*.cpp", "./Facenet/*.c")
file(GLOB_RECURSE UI_source "./UI/*.cpp", "./UI/*.c")
file(GLOB_RECURSE Sql_source "./Sql/*.cpp", "./Sql/*.c")

file(GLOB_RECURSE Yolo_header "./Yolov8/*.h", "./Yolov8/*.hpp")
file(GLOB_RECURSE Ffmpeg_header "./Ffmpeg/*.h", "./Ffmpeg/*.hpp")
file(GLOB_RECURSE Facenet_header "./Facenet/*.h", "./Facenet/*.hpp")
file(GLOB_RECURSE UI_header "./UI/*.h", "./UI/*.hpp")
file(GLOB_RECURSE Sql_header "./Sql/*.h", "./Sql/*.hpp")

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
)


# Include directories for headers
# include_directories(
#     ${OpenCV_INCLUDE_DIRS}
#     ${FFMPEG_PREFIX_PATH}/include
#     ${onnxDir}/include
# )

# Link directories for libraries
# link_directories(
#     ${onnxDir}/lib
#     ${FFMPEG_PREFIX_PATH}/lib
# )

# Add executable target
add_executable(FaceRecognition ${PROJECT_SOURCES})

# Link libraries to the target
target_link_libraries(FaceRecognition PRIVATE
    ${OpenCV_LIBS}
    onnxruntime
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
