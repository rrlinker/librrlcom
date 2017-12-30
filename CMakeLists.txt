cmake_minimum_required(VERSION 3.7)

project(rlcom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCE_FILES
    "*.cpp"
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

