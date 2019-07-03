# find cpunit
#
# exports:
#
#   CppUnit_FOUND
#   CppUnit_INCLUDE_DIRS
#   CppUnit_LIBRARIES
#

include(FindPkgConfig)
include(FindPackageHandleStandardArgs)

# Use pkg-config to get hints about paths
pkg_check_modules(CppUnit_PKGCONF REQUIRED cppunit)

# Include dir
find_path(CppUnit_INCLUDE_DIR
  NAMES cppunit/TestRunner.h
  PATHS ${CppUnit_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(CppUnit_LIBRARY
  NAMES cppunit
  PATHS ${CppUnit_PKGCONF_LIBRARY_DIRS}
)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CppUnit DEFAULT_MSG CppUnit_LIBRARY CppUnit_INCLUDE_DIR)


if(CppUnit_PKGCONF_FOUND)
  set(CppUnit_LIBRARIES ${CppUnit_LIBRARY} ${CppUnit_PKGCONF_LIBRARIES})
  set(CppUnit_INCLUDE_DIRS ${CppUnit_INCLUDE_DIR} ${CppUnit_PKGCONF_INCLUDE_DIRS})
  set(CppUnit_FOUND yes)
else()
  set(CppUnit_LIBRARIES)
  set(CppUnit_INCLUDE_DIRS)
set(CppUnit_FOUND no)
endif()
