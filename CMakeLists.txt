#####################################################################
# This file is part of the IC reverse engineering tool Degate.
#
# Copyright 2008, 2009, 2010 by Martin Schobert
# Copyright 2019-2020 Dorian Bachelot
#
# Degate is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# Degate is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with degate. If not, see <http://www.gnu.org/licenses/>.
#
#####################################################################

cmake_minimum_required(VERSION 3.12.0)
project(DEGATE)

#
# Cpp version
#
set(CMAKE_CXX_STANDARD 11)
message(STATUS "Using compiler: ${CMAKE_CXX_COMPILER}")

set(CMAKE_VERBOSE_MAKEFILE true)

#
# Set flags
#
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


############################################################################
############################ Dependencies ##################################
############################################################################

#
# Turn on automatic invocation of the MOC, UIC & RCC
#
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#
# Group Qt files
#
source_group("qt" REGULAR_EXPRESSION "(resources.qrc|moc.*.cpp|qrc_resources.cpp)")
source_group("qt\\languages" REGULAR_EXPRESSION "(.*.qm|translations.qrc)")
source_group("res" REGULAR_EXPRESSION "(resource.rc)")
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
################################ Main ######################################
############################################################################

#
# Configuration
#
if (CMAKE_SIZEOF_VOID_P MATCHES 8) # if x64
    message(STATUS "x64 configuration")
    set(CONFIGURATION "x64")
else() # if x86
    message(STATUS "x86 configuration")
    set(CONFIGURATION "x86")
endif()

#
# Version
#
include(${CMAKE_CURRENT_SOURCE_DIR}/etc/cmake/ConfigureFiles.cmake)

#
# Version command
#
add_custom_command(
        OUTPUT "Degate version" # Not an error, we need a dummy output to force command execution for every build
        COMMAND ${CMAKE_COMMAND} -DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
                                 -DCMAKE_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}"
                                 -DCONFIGURATION="${CONFIGURATION}"
                                 -P "${CMAKE_CURRENT_SOURCE_DIR}/etc/cmake/ConfigureFiles.cmake"
        DEPENDS "${PROJECT_SOURCE_DIR}/VERSION"
)

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
        degate_de.ts
        degate_ru.ts
        degate_es.ts
        degate_it.ts
        degate_ko.ts
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
# MacOS set icns icon.
#
set(MACOSX_BUNDLE_ICON_FILE "degate.icns")

#
# MacOS copy icns icon.
#
if (APPLE)
    file(COPY ${PROJECT_SOURCE_DIR}/etc/icons/logo/degate.icns DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/out/bin/Degate.app/Contents/Resources/)
endif ()

#
# Link DegateCore
#
add_library(DegateCore STATIC ${SRC_FILES} res/resources.qrc res/qdarkstyle/style.qrc ${CMAKE_BINARY_DIR}/translations.qrc ${TRANSLATION_FILES} ${QM_FILES} "Degate version")
target_link_libraries(DegateCore ${LIBS})

#
# Link main (Degate)
#
add_executable(Degate "src/Main.cc" res/resources.qrc res/qdarkstyle/style.qrc ${CMAKE_BINARY_DIR}/translations.qrc ${TRANSLATION_FILES} ${QM_FILES} "res/resource.rc")
target_link_libraries(Degate ${LIBS} DegateCore)

#
# Activate bundle for MacOS.
#
set_target_properties(Degate PROPERTIES MACOSX_BUNDLE TRUE)

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
################################ Doc #######################################
############################################################################

find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    message(STATUS "Doxygen found")

    add_custom_target(Documentation ALL
            COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/doc/config/DoxyFile"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating documentation with Doxygen."
            SOURCES "Degate version")

else()
    message(STATUS "Doxygen not found")
endif()