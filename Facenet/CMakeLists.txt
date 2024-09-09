cmake_minimum_required(VERSION 3.5)
project(FaceNet VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set paths to dependencies
set(OpenCV_DIR "D:/Program/OpenCV/opencv/build/x64/vc15/lib")
set(onnxDir "D:/Program/onnxruntime-win-x64-1.10.0/")

# Find required packages
find_package(OpenCV REQUIRED)

set(PROJECT_SOURCES
    main.cpp
    widget.cpp
    widget.h
)

# Include directories for headers
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${onnxDir}/include
)

# Link directories for libraries
link_directories(
    ${onnxDir}/lib
)

# Add executable target
add_executable(FaceNet ${PROJECT_SOURCES})

# Link libraries to the target
target_link_libraries(FaceNet PRIVATE
    ${OpenCV_LIBS}
    onnxruntime
)

# Optionally, set output directory for the executable
set_target_properties(FaceNet PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
