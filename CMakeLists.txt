cmake_minimum_required(VERSION 3.0)
project(DEGATE)

set(CMAKE_CXX_STANDARD 11)
message(STATUS "Using compiler: ${CMAKE_CXX_COMPILER}")

set(CMAKE_VERBOSE_MAKEFILE true)

if (NOT MSVC)
	add_definitions(-Wall -g -fno-inline -std=c++11 -DBOOST_NO_CXX11_SCOPED_ENUMS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
	add_definitions(-DBOOST_NO_CXX11_SCOPED_ENUMS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif()

# property for folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(src)

#INSTALL(FILES degate.png DESTINATION share/pixmaps)
#INSTALL(FILES degate.desktop DESTINATION share/applications)

if (CMAKE_SIZEOF_VOID_P MATCHES 8) # if x64
		message("-- x64 configuration")
	else() # if x86
		message("-- x86 configuration")
endif()
