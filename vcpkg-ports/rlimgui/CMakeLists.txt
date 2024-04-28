cmake_minimum_required(VERSION 3.5)

project(rlimgui)

include(GNUInstallDirs)

find_package(raylib CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

set (INSTALLABLE_HEADERS
	"rlImGui.h"
	"rlImGuiColors.h")

set (INSTALLABLE_HEADERS_EXTRAS
	"extras/FA6FreeSolidFontData.h"
	"extras/IconsFontAwesome6.h")

set(SOURCE_FILES
	"rlImGui.cpp")

set (INSTALLABLE_DOCFILES
  "README.md"
  "LICENSE"
  "extras/FontAwsome_LICENSE.txt")

SET(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Output directory for libraries")
SET(CMAKE_INSTALL_BINDIR bin CACHE PATH "Output directory for binaries")
SET(CMAKE_INSTALL_INCLUDEDIR include CACHE PATH "Output directory for header files")
SET(CMAKE_INSTALL_DOCDIR share/doc CACHE PATH "Output directory for documentation and license files")

add_library(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC
	raylib
	imgui::imgui)

INSTALL(FILES ${INSTALLABLE_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rlimgui)
INSTALL(FILES ${INSTALLABLE_HEADERS_EXTRAS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rlimgui/extras)
INSTALL(FILES ${INSTALLABLE_DOCFILES} DESTINATION ${CMAKE_INSTALL_DOCDIR}/rlimgui)

INSTALL(TARGETS ${PROJECT_NAME}
	EXPORT ${PROJECT_NAME}_targets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

INSTALL(EXPORT ${PROJECT_NAME}_targets
		FILE ${PROJECT_NAME}-targets.cmake
		NAMESPACE ${PROJECT_NAME}::
		DESTINATION share/${PROJECT_NAME}
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
"include(CMakeFindDependencyMacro)\n"
"find_dependency(raylib)\n"
"find_dependency(imgui)\n"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}-targets.cmake\")\n"
)

INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        DESTINATION share/${PROJECT_NAME})

OPTION(BUILD_EXAMPLES "Build the examples" OFF)

if (${BUILD_EXAMPLES})
    add_executable(${PROJECT_NAME}-example-simple "examples/simple.cpp")
	add_executable(${PROJECT_NAME}-example-imgui_style "examples/imgui_style_example.cpp")
	add_executable(${PROJECT_NAME}-example-editor "examples/editor.cpp")
    
	include_directories(${PROJECT_NAME}-example-simple ".")
	include_directories(${PROJECT_NAME}-example-imgui_style ".")
	include_directories(${PROJECT_NAME}-example-editor ".")
	
    target_link_libraries(${PROJECT_NAME}-example-simple ${PROJECT_NAME})
	target_link_libraries(${PROJECT_NAME}-example-imgui_style ${PROJECT_NAME})
	target_link_libraries(${PROJECT_NAME}-example-editor ${PROJECT_NAME})

endif()
