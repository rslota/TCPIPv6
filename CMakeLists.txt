cmake_minimum_required(VERSION 2.6)
project(TCPIPStack C)

# The version number.
set(TCPIPStack_VERSION_MAJOR 0)
set(TCPIPStack_VERSION_MINOR 1)
set(TCPIPStack_VERSION_PATCH 1)
set(TCPIPStack_VERSION $(TCPIPStack_VERSION_MAJOR).$(TCPIPStack_VERSION_MINOR).$(TCPIPStack_VERSION_PATCH))

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Os -ggdb")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

add_subdirectory(src)
