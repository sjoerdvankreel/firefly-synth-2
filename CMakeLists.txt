cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15 CACHE STRING "Minimum macOS version")

project(playground-plug)
set_property(GLOBAL PROPERTY USE_FOLDERS YES)

set (SRC_DIR "${CMAKE_SOURCE_DIR}/src")
file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.*")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${SRC_DIR} FILES ${SRC_FILES})
endif()
add_executable(playground-plug ${SRC_FILES})