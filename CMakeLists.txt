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

set(HEADER_FILES
    "include/rrlinker/com/address.hpp"
    "include/rrlinker/com/bound_check.hpp"
    "include/rrlinker/com/connection.hpp"
    "include/rrlinker/com/courier.hpp"
    "include/rrlinker/com/message_definitions.hpp"
    "include/rrlinker/com/message.hpp"
    "include/rrlinker/com/raw_courier.hpp"
)

add_library(${PROJECT_NAME} STATIC
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    PUBLIC_HEADER "${HEADER_FILES}"
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

include(GNUInstallDirs)
install(
    TARGETS ${PROJECT_NAME}
    EXPORT com
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/rrlinker/com"
)
install(EXPORT com DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake")
