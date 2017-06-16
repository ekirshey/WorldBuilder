include_directories(
	${SDL2_INCLUDE_DIR}
	${GLEW_INCLUDE_DIR}
	${GLM_INCLUDE_DIR}
	${ASSIMP_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/include/imgui
)

file( GLOB SRCS 
	#Includes
	${PROJECT_SOURCE_DIR}/include/*.h
	${PROJECT_SOURCE_DIR}/include/imgui/*.h

	#Source
	${PROJECT_SOURCE_DIR}/src/*.cpp
	
	#imgui 
	${PROJECT_SOURCE_DIR}/src/imgui/*.cpp
)

add_executable(WorldBuilder
	${SRCS} 
)

target_link_libraries (WorldBuilder LINK_PUBLIC ${SDL2_LIBRARY} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY} ${ASSIMP_LIBRARY_DEBUG})

foreach(FILE ${SRCS}) 
	# Get the directory of the source file
	get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
	
	# Remove common directory prefix to make the group
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
	string(REPLACE "${PROJECT_SOURCE_DIR}/include" "" GROUP "${GROUP}")

	# Make sure we are using windows slashes
	string(REPLACE "/" "\\" GROUP "${GROUP}")

	# Group into "Source Files" and "Header Files"
	if ("${FILE}" MATCHES ".*\\.cpp")
	   set(GROUP "Source Files${GROUP}")
	elseif("${FILE}" MATCHES ".*\\.h")
	   set(GROUP "Header Files${GROUP}")
	endif()

	source_group("${GROUP}" FILES "${FILE}")
endforeach()