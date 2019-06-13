cmake_minimum_required(VERSION 3.14)

project(
    librlcom
    VERSION 0.1
    DESCRIPTION "Communication library for Remote Run-time Linker."
    LANGUAGES CXX
)

include("${CMAKE_SOURCE_DIR}/../cmake/find_libs_dir.cmake")
include("${CMAKE_SOURCE_DIR}/../cmake/find_arch.cmake")

file(GLOB SOURCE_FILES
    "*.cpp"
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE "${PROJECT_SOURCE_DIR}/.."
)
