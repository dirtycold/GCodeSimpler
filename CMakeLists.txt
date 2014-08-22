project( GCodeSimpler )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE RelWithDebugInfo )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	)
set ( GCodeSimpler_HDRS
	
	)

set ( GCodeSimpler_SRCS
	main.cpp
	gcodesimplerwidget.cpp
	gcodesimpler.cpp
	)

set ( GCodeSimpler_TRS
	GCodeSimpler.ts
	GCodeSimpler_zh_CN.ts
	)
QT4_ADD_TRANSLATION(TRS ${GCodeSimpler_TRS})

set ( GCodeSimpler_MOCS
	gcodesimplerwidget.h
	gcodesimpler.h
	)
QT4_WRAP_CPP(MOCS ${GCodeSimpler_MOCS})

add_executable ( GCodeSimpler ${GCodeSimpler_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( GCodeSimpler  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )
