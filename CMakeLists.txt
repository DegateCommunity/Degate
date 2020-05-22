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

#
# Property for folders
#
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

############################ Dependencies ##################################

# Turn on automatic invocation of the MOC, UIC & RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

############# Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS filesystem system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
set(LIBS ${Boost_LIBRARIES})

############# Qt
find_package(Qt5 COMPONENTS Core Widgets Gui Xml OpenGL Concurrent REQUIRED)
set(LIBS ${LIBS} Qt5::Widgets Qt5::Gui Qt5::Core Qt5::Xml Qt5::OpenGL Qt5::Concurrent)

############################################################################

#
# The source files
#
file(GLOB_RECURSE SRC_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" LIST_DIRECTORIES false 
	"src/*.cc"
	"src/*.cpp"
	"src/*.h"
	"src/*.hpp"
)

#
# Include directorie
#
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

#
# Defines groups (to respect folders hierarchy)
#
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "src" FILES ${SRC_FILES})
source_group("qt" REGULAR_EXPRESSION "(resources.qrc|mocs_compilation.cpp|qrc_resources.cpp)")

#
# Link
#
add_executable(Degate ${SRC_FILES} res/resources.qrc)
target_link_libraries(Degate ${LIBS})

#
# Installation specifications
#
install (TARGETS Degate DESTINATION out/bin)

#
# Output specifications
#
set_target_properties(Degate
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "out/lib"
    LIBRARY_OUTPUT_DIRECTORY "out/lib"
    RUNTIME_OUTPUT_DIRECTORY "out/bin"
)

#
# No console on Windows when release.
#
if(MSVC) # If using the VS compiler...
    set_target_properties(Degate PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

############################################################################
#
# Documentation
#
#find_package(Doxygen QUIET)
#IF (DOXYGEN_FOUND)
#   add_custom_target(libdoc 
#     COMMAND ${DOXYGEN_EXECUTABLE} "${PROJECT_SOURCE_DIR}/doc/Doxyfile.libdegate"
#     SOURCES ${SOURCES})
#else()
#   message("Warning: Doxygen not found. Ignoring. You should "
#   	"install the doxygen package to generate the documentation for libdegate.")
#ENDIF(DOXYGEN_FOUND)

if (CMAKE_SIZEOF_VOID_P MATCHES 8) # if x64
		message("-- x64 configuration")
	else() # if x86
		message("-- x86 configuration")
endif()
