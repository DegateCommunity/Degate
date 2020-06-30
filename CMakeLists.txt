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

#
# Group Qt files
#
source_group("qt" REGULAR_EXPRESSION "(resources.qrc|moc.*.cpp|qrc_resources.cpp)")
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "Moc")
set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "qt")

############# Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS filesystem system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
set(LIBS ${Boost_LIBRARIES})

############# Qt
find_package(Qt5 COMPONENTS Core Widgets Gui Xml OpenGL Concurrent LinguistTools REQUIRED)
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
# Translation
#
SET(TRANSLATION_FILES
		degate_en.ts
		degate_fr.ts
)

list(TRANSFORM TRANSLATION_FILES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/res/languages/)

# Redefine "qt5_create_translation" function because of a bug (it originally parse all boost files, and make the build VERY slow)
include(${CMAKE_CURRENT_SOURCE_DIR}/etc/cmake/Qt5LinguistToolsMacros.cmake)

qt5_create_translation(QM_FILES ${SRC_FILES} ${TRANSLATION_FILES} OPTIONS -no-obsolete)

configure_file(res/languages/translations.qrc ${CMAKE_BINARY_DIR} COPYONLY)

#
# Include directories
#
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

#
# Defines groups (to respect folders hierarchy)
#
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "src" FILES ${SRC_FILES})

#
# Remove Main.cpp file
#
list(REMOVE_ITEM SRC_FILES "src/Main.cc")

#
# Link DegateCore
#
add_library(DegateCore STATIC ${SRC_FILES} res/resources.qrc ${CMAKE_BINARY_DIR}/translations.qrc ${QM_FILES})
target_link_libraries(DegateCore ${LIBS})

#
# Link main (Degate)
#
add_executable(Degate "src/Main.cc" res/resources.qrc ${CMAKE_BINARY_DIR}/translations.qrc ${QM_FILES})
target_link_libraries(Degate ${LIBS} DegateCore)

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

set_target_properties(DegateCore
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
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Degate)
endif()

#
# Tests
#
add_subdirectory(tests)

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
