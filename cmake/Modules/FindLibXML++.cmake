# find libxml++
#
# exports:
#
#   LibXML++_FOUND
#   LibXML++_INCLUDE_DIRS
#   LibXML++_LIBRARIES
#

include(FindPkgConfig)
include(FindPackageHandleStandardArgs)

# Use pkg-config to get hints about paths
pkg_check_modules(LibXML++_PKGCONF REQUIRED libxml++-2.6)

# Include dir
find_path(LibXML++_INCLUDE_DIR
  NAMES libxml++/libxml++.h
  PATHS ${LibXML++_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(LibXML++_LIBRARY
  NAMES xml++ xml++-2.6
  PATHS ${LibXML++_PKGCONF_LIBRARY_DIRS}
)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibXML++ DEFAULT_MSG LibXML++_LIBRARY LibXML++_INCLUDE_DIR)


if(LibXML++_PKGCONF_FOUND)
  set(LibXML++_LIBRARIES ${LibXML++_LIBRARY} ${LibXML++_PKGCONF_LIBRARIES})
  set(LibXML++_INCLUDE_DIRS ${LibXML++_INCLUDE_DIR} ${LibXML++_PKGCONF_INCLUDE_DIRS})
  set(LibXML++_FOUND yes)
else()
  set(LibXML++_LIBRARIES)
  set(LibXML++_INCLUDE_DIRS)
  set(LibXML++_FOUND no)
endif()

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
#set(LibXML++_PROCESS_INCLUDES LibXML++_INCLUDE_DIR)
#set(LibXML++_PROCESS_LIBS LibXML++_LIBRARY)

