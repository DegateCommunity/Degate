find_package(CppUnit)

if(CppUnit_FOUND)

	ENABLE_TESTING()


	include_directories(${CppUnit_INCLUDE_DIRS})
	set(LIBS ${LIBS} ${CppUnit_LIBRARIES})

	include_directories(. ../lib)

	find_package(LibXML++ REQUIRED)
	include_directories(${LibXML++_INCLUDE_DIRS})
	set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

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
	      ImageProcessingTest.cc
	      )

	set(TESTMAIN main.cc)


	add_executable(libdegate_test ${TESTCASES} ${TESTMAIN})
	target_link_libraries(libdegate_test ${LIBS} degate)

	add_test(libdegatetest {EXECUTABLE_OUTPUT_PATH}/libdegate_test)

else()
	message("Cppunit framework not found. Will not build test "
			 "suite. Please install the cppunit package in order to build tests")
endif(CppUnit_FOUND)
