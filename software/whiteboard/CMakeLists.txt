cmake_minimum_required(VERSION 3.16)
project(Whiteboard LANGUAGES C CXX)

set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")

# Define Qt cmake modules path if not already done
if (DEFINED ENV{Qt6_DIR} AND NOT DEFINED Qt6_DIR)
    STRING(REPLACE "\"" "" Qt6_DIR $ENV{Qt6_DIR})
endif ()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/apps)
