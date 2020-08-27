enable_testing()
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/catch2/")

#
# The tests source files
#
file(GLOB_RECURSE TEST_SRC_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" LIST_DIRECTORIES false
	"src/*.cc"
	"src/*.cpp"
	"src/*.h"
	"src/*.hpp"
)

#
# Include directories
#
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../src")

#
# Defines groups (to respect folders hierarchy)
#
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "src" FILES ${TEST_SRC_FILES})

#
# Link
#
add_executable(DegateTests ${TEST_SRC_FILES})
target_link_libraries(DegateTests ${LIBS} DegateCore)

#
# Output specifications
#
set_target_properties(DegateTests
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "out/lib"
	LIBRARY_OUTPUT_DIRECTORY "out/lib"
	RUNTIME_OUTPUT_DIRECTORY "out/bin"
)

#
# Link Catch2 tests to CTest
#
include(catch2/Catch.cmake)
catch_discover_tests(DegateTests)

#
# Copy test files
#
add_custom_command(
		TARGET DegateTests POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/tests/tests_files/
		$<TARGET_FILE_DIR:DegateTests>/tests_files/)