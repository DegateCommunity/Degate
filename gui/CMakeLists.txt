include_directories(. ${DEGATE_SOURCE_DIR}/lib)
link_directories(${DEGATE_BINARY_DIR}/degate)

#
# lookup dependencies
#

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-2.4)
include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS}) 
set(LIBS ${LIBS} ${GTKMM_LIBRARIES})

pkg_check_modules(GTHREAD gthread-2.0)
include_directories(${GTHREAD_INCLUDE_DIRS})
link_directories(${GTHREAD_LIBRARY_DIRS}) 
set(LIBS ${LIBS} ${GTHREAD_LIBRARIES})

pkg_check_modules(CAIROMM cairomm-1.0)
include_directories(${CAIROMM_INCLUDE_DIRS})
link_directories(${CAIROMM_LIBRARY_DIRS}) 
set(LIBS ${LIBS} ${CAIROMM_LIBRARIES})

pkg_check_modules(LIBGLADEMM glibmm-2.4)
include_directories(${LIBGLADEMM_INCLUDE_DIRS})
link_directories(${LIBGLADEMM_LIBRARY_DIRS}) 
set(LIBS ${LIBS} ${LIBGLADE_LIBRARIES})

pkg_check_modules(GLIBMM libglademm-2.4)
include_directories(${GLIBMM_INCLUDE_DIRS})
link_directories(${GLIBMM_LIBRARY_DIRS}) 
set(LIBS ${LIBS} ${GLIBMM_LIBRARIES})


pkg_check_modules(LIBXML++ libxml++-2.6)
include_directories(${LIBXML++_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LIBXML++_LIBRARIES})

#pkg_check_modules(GTKGLEXTMM gtkglextmm-1.2 )
#include_directories(${GTKGLEXTMM_INCLUDE_DIRS})
#link_directories(${GTKGLEXTMM_LIBRARY_DIRS}) 
#set(LIBS ${LIBS} ${GTKGLEXTMM_LIBRARIES})

#
# sources
#
set(DEGATE_SRC 
	       main.cc 
	       SplashWin.cc
	       InProgressWin.cc
	       SetOrientationWin.cc
	       ProjectSettingsWin.cc
	       GladeFileLoader.cc
	       GridConfigWin.cc
	       HlObjectSet.cc
	       GenericTextInputWin.cc

	       GateConfigWin.cc
	       GateListWin.cc

	       GateSelectWin.cc

	       PortColorsWin.cc
	       PortSelectWin.cc

	       LayerConfigWin.cc
	       AnnotationListWin.cc
	       ModuleWin.cc
	       SelectModuleWin.cc

	       NewProjectWin.cc
	       ObjectMatchingWin.cc
	       ConnectionInspectorWin.cc
	       MenuManager.cc	       
	       
	       TemplateMatchingParamsWin.cc
	       WireMatchingParamsWin.cc

	       RecognitionManager.cc
	       TemplateMatchingGUI.cc
	       WireMatchingGUI.cc

	       AppHelper.cc
	       ImageWin.cc
	       MainWin.cc

)

add_executable(degate_bin ${DEGATE_SRC})

# set libraries for the main executable
target_link_libraries(degate_bin ${LIBS} degate)

