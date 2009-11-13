include_directories (. /opt/local/include /opt/local/include/freetype2 /usr/include/freetype2)

#
# Check libraries.
#

find_package(LibXML++ REQUIRED)
include_directories(${LibXML++_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

find_package(TIFF)
include_directories(${TIFF_INCLUDE_DIR})
set(LIBS ${LIBS} ${TIFF_LIBRARIES})

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS filesystem system)
include_directories(${Boost_INCLUDE_DIR})
set(LIBS ${LIBS} ${Boost_LIBRARIES})

find_package(Freetype)
include_directories(${FREETYPE_INCLUDE_DIR})
message(STATUS ${FREETYPE_INCLUDE_DIR})
set(LIBS ${LIBS} ${FREETYPE_LIBRARIES})

#set(LIBS ${LIBS} "-lshogun")

#
# Check endianess
#
INCLUDE(${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(WORDS_BIGENDIAN)

#
# The source files
#
set(SOURCES

	#
	# genric stuff
	#

	debug.cc
	FileSystem.cc
	RegularGrid.cc
	IrregularGrid.cc
	Configuration.cc

	#
	# shape related
	#

	BoundingBox.cc
	Circle.cc
	Point.cc
	Line.cc
	Rectangle.cc

	#
	# logic model related
	#

	LogicModel.cc
	LogicModelHelper.cc
	Net.cc
	Layer.cc
	Project.cc
	Wire.cc
	Via.cc
	Annotation.cc
	LogicModelObjectBase.cc
	ConnectedLogicModelObject.cc
	PlacedLogicModelObject.cc
	GatePort.cc
	Gate.cc
	GateTemplate.cc
	GateLibrary.cc

	#
	# importer / exporter
	#

	Importer.cc
	XMLImporter.cc
	ProjectImporter.cc
	LogicModelImporter.cc
	GateLibraryImporter.cc
	GateLibraryExporter.cc
	LogicModelExporter.cc
	ProjectExporter.cc
	DOTExporter.cc
	DOTAttributes.cc
	LogicModelDOTExporter.cc

	#
	# Images
	#
	renderer.cc
	FilterKernel.cc
	WireMatchingSimple.cc
	WireMatchingCanny.cc
	TemplateMatching.cc
)


add_library(degate SHARED ${SOURCES} )
target_link_libraries(degate ${LIBS})

#
# Documentation
#

find_package(Doxygen QUIET)
IF (DOXYGEN_FOUND)
   add_custom_target(libdoc 
 	COMMAND ${DOXYGEN_EXECUTABLE} "${PROJECT_SOURCE_DIR}/doc/Doxyfile.libdegate"}
	 SOURCES ${SOURCES})
else()
   message("Warning: Doxygen not found. Ignoring. You should "
   	"install the doxygen package to generate the documentation for libdegate.")
ENDIF(DOXYGEN_FOUND)
