find_package(PkgConfig)

pkg_check_modules(CPPUNIT cppunit)


if(CPPUNIT_FOUND)

	ENABLE_TESTING()

        include_directories(${CPPUNIT_INCLUDE_DIRS})
        link_directories(${CPPUNIT_LIBRARY_DIRS}) 
        set(LIBS ${LIBS} ${CPPUNIT_LIBRARIES})

        pkg_check_modules(LIBXML++ libxml++-2.6)
        include_directories(${LIBXML++_INCLUDE_DIRS})

	set(Boost_USE_STATIC_LIBS   OFF)
	find_package(Boost REQUIRED COMPONENTS system)
	if(Boost_FOUND)
	  include_directories(${Boost_INCLUDE_DIRS})
	  set(LIBS ${LIBS} ${Boost_LIBRARIES})
	endif()

	include_directories(. ../lib)


	set(TESTCASES 
	      FileSystemTest.cc
	      ImageTest.cc
	      MemoryMapTest.cc
	      ProjectImporterTest.cc
	      LogicModelImporterTest.cc
	      GateLibraryImporterTest.cc
	      QuadTreeTest.cc
##	      ShapeTest.cc
	      LMOinQTreeTest.cc

	      LogicModelTest.cc

	      GateLibraryExporterTest.cc
	      LogicModelExporterTest.cc
	      ProjectExporterTest.cc
	      LogicModelDOTExporterTest.cc

	      ScalingManagerTest.cc
#	      ImageProcessingTest.cc

	      LookupSubcircuitTest.cc
	      )

	set(TESTMAIN main.cc)


	add_executable(libdegate_test ${TESTCASES} ${TESTMAIN})
	target_link_libraries(libdegate_test ${LIBS} degate)

	add_test(libdegatetest {EXECUTABLE_OUTPUT_PATH}/libdegate_test)

else()
	message("Cppunit framework not found. Will not build test "
			 "suite. Please install the cppunit package in order to build tests")
endif(CPPUNIT_FOUND)
