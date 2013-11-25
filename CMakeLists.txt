cmake_minimum_required(VERSION 2.6)
project(TCPIPStack C)

# The version number.
set(TCPIPStack_VERSION_MAJOR 0)
set(TCPIPStack_VERSION_MINOR 1)
set(TCPIPStack_VERSION_PATCH 1)
set(TCPIPStack_VERSION $(TCPIPStack_VERSION_MAJOR).$(TCPIPStack_VERSION_MINOR).$(TCPIPStack_VERSION_PATCH))

add_definitions(
    -std=c11 
    -g
)

add_subdirectory(src)

add_library(tcpip_stack SHARED ${PROJECT_SOURCE_DIR}/src/session.c)
target_link_libraries(tcpip_stack
    internet_layer
)