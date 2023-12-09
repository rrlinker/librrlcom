cmake_minimum_required(VERSION 3.14)

project(
    librrlcom
    VERSION 0.1
    DESCRIPTION "Communication library for Remote Run-time Linker."
    LANGUAGES CXX
)

set(SOURCE_FILES
    "lib/address.cpp"
    "lib/courier.cpp"
    "lib/raw_courier.cpp"
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC include
)
