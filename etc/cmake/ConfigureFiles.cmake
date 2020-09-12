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

# Read version file
file(READ "${PROJECT_SOURCE_DIR}/VERSION" VERSION_FILE_CONTENT)
string(TIMESTAMP CURRENT_DATE "%Y-%m-%d")

# Create the list
STRING(REGEX REPLACE "\n" ";" VERSION_FILE_CONTENT "${VERSION_FILE_CONTENT}")

# Clear empty list entries
STRING(REGEX REPLACE ";;" ";" VERSION_FILE_CONTENT "${VERSION_FILE_CONTENT}")

# Remove lines that start with a '#'
foreach(LINE ${VERSION_FILE_CONTENT})
    if("${LINE}" MATCHES "^(#.*|\\n)")
        list(REMOVE_ITEM VERSION_FILE_CONTENT "${LINE}")
        continue()
    endif()
endforeach()

# Read the Degate version
list(GET VERSION_FILE_CONTENT 0 DEGATE_VERSION)

# Read the Degate version release date
list(GET VERSION_FILE_CONTENT 1 DEGATE_RELEASE_DATE)

# Print information
if("${PROJECT_NAME}" STREQUAL "")
    # If the project name is not set, then we are in the build process
    execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold
            "======================================="
            "\t Degate version: ${DEGATE_VERSION}"
            "\t Release date:   ${DEGATE_RELEASE_DATE}"
            "\t Build date:     ${CURRENT_DATE}"
            "\t OS:			 ${CMAKE_SYSTEM_NAME}"
            "\t Configuration:  ${CONFIGURATION}"
            "=======================================")
else()
    # Otherwise we are in the CMakeLists load/reload process
    message(STATUS "Degate version: ${DEGATE_VERSION}")
    message(STATUS "Release date: ${DEGATE_RELEASE_DATE}")
endif()

# If the release date is "Unreleased" then take the current date (build date)
if(DEGATE_RELEASE_DATE MATCHES "Unreleased")
    set(DEGATE_RELEASE_DATE "${CURRENT_DATE}")
endif()

# Configure each file
configure_file("${PROJECT_SOURCE_DIR}/etc/config/Version.config" "${PROJECT_SOURCE_DIR}/src/Core/Version.h")
configure_file("${PROJECT_SOURCE_DIR}/etc/installer/packages/Degate/meta/package.config" "${PROJECT_SOURCE_DIR}/etc/installer/packages/Degate/meta/package.xml")
configure_file("${PROJECT_SOURCE_DIR}/etc/installer/config/config.config" "${PROJECT_SOURCE_DIR}/etc/installer/config/config.xml")