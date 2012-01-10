find_package(PkgConfig)


pkg_check_modules(LIBXML++ libxml++-2.6)
include_directories(${LIBXML++_INCLUDE_DIRS})

find_package(Boost REQUIRED COMPONENTS program_options)
if(Boost_FOUND)
        include_directories(${Boost_INCLUDE_DIRS})
        link_directories(${Boost_LIBRARY_DIRS}) 
        set(LIBS ${LIBS} ${Boost_LIBRARIES})
endif()

find_package(XMLRPC COMPONENTS c++2 client)
if(XMLRPC_FOUND)
	include_directories(${XMLRPC_INCLUDE_DIRS})
	set(LIBS ${LIBS} ${XMLRPC_LIBRARIES})
endif()



include_directories(. ../../lib)

set(TOOL_NAME gate_lib_doc)
set(TOOL_SRC ${TOOL_NAME}.cc)

add_executable(${TOOL_NAME} ${TOOL_SRC})
target_link_libraries(${TOOL_NAME} ${LIBS} degate)

