cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)

project(WorldBuilder)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

message("${CMAKE_BINARY_DIR}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Find needed libraries
find_package(SDL2)
find_package(OpenGL)
find_package(GLEW)
find_package(GLM)
find_package(Assimp)

add_subdirectory(src)
