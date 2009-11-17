PROJECT(DEGATE)
cmake_minimum_required(VERSION 2.6)

message(STATUS "Using compiler: ${CMAKE_CXX_COMPILER}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")


set(CMAKE_VERBOSE_MAKEFILE true)

#
# global flags
#
#add_definitions(-ggdb -Wall -pg)
#set(CMAKE_EXE_LINKER_FLAGS -pg)
#set(CMAKE_SHARED_LINKER_FLAGS -pg)

add_definitions(-Wall -O3 -ggdb)
#set(CMAKE_EXE_LINKER_FLAGS)
#set(CMAKE_SHARED_LINKER_FLAGS -O3)


add_subdirectory(lib)
add_subdirectory(libtest)
add_subdirectory(gui )


