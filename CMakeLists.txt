PROJECT(DEGATE)
cmake_minimum_required(VERSION 2.6)

message(STATUS "Using compiler: ${CMAKE_CXX_COMPILER}")

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")


set(CMAKE_VERBOSE_MAKEFILE true)

#
# global flags
#
#add_definitions(-ggdb -Wall -O3 -pg)
#set(CMAKE_EXE_LINKER_FLAGS -pg)
#set(CMAKE_SHARED_LINKER_FLAGS -pg)

add_definitions(-Wall -g -fno-inline -std=c++0x)
#set(CMAKE_EXE_LINKER_FLAGS)
#set(CMAKE_SHARED_LINKER_FLAGS -O3)


#SET(Boost_ADDITIONAL_VERSIONS "1.46" "1.46.1")
#SET(BOOST_ROOT "/home/martin/boost1461/")
#SET(Boost_NO_SYSTEM_PATHS "true")

add_subdirectory(lib)
add_subdirectory(libtest)
add_subdirectory(tools/gate_lib_documentation)
add_subdirectory(tools/determine_module_ports)
add_subdirectory(tools/export_module)
add_subdirectory(gui)


INSTALL(FILES degate.png DESTINATION share/pixmaps)
INSTALL(FILES degate.desktop DESTINATION share/applications)
