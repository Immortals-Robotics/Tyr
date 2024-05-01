project(${CMAKE_PROJECT_NAME}-cli)

set(HEADER_FILES
	pch.h
)

set(SOURCE_FILES
	main.cpp
)

add_executable(${PROJECT_NAME}
	${HEADER_FILES}
	${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
	${CMAKE_PROJECT_NAME}-common
	${CMAKE_PROJECT_NAME}-vision
	${CMAKE_PROJECT_NAME}-referee
	${CMAKE_PROJECT_NAME}-sender
	${CMAKE_PROJECT_NAME}-soccer)

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

target_precompile_headers(${PROJECT_NAME} PRIVATE "pch.h")

install(TARGETS ${PROJECT_NAME})

