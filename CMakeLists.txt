# global
cmake_minimum_required(VERSION 3.21)
project(playground-plug)

# build
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15 CACHE STRING "Minimum macOS version")
set_property(GLOBAL PROPERTY USE_FOLDERS YES)

# vst3
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/lib/vst3/cmake/modules")
set(VST_SDK TRUE)
set(SMTG_ENABLE_VSTGUI_SUPPORT OFF)
set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF)
set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF)
include(SMTG_VST3_SDK)
smtg_setup_platform_toolset()
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/vst3/base")
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/vst3/public.sdk")
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/vst3/pluginterfaces")

# playground
set (SRC_DIR "${CMAKE_SOURCE_DIR}/src/playground_plug")
file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.*")
add_library(playground_plug STATIC ${SRC_FILES})
target_include_directories(playground_plug PRIVATE ${CMAKE_SOURCE_DIR}/src/playground_plug)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${SRC_DIR} FILES ${SRC_FILES})
endif()

# playground vst3
set (VST3_SRC_DIR "${CMAKE_SOURCE_DIR}/src/playground_plug_vst3")
file(GLOB_RECURSE VST3_SRC_FILES "${VST3_SRC_DIR}/*.*")
add_library(playground_plug_vst3 SHARED ${VST3_SRC_FILES})
target_link_libraries(playground_plug_vst3 playground_plug)
target_link_libraries(playground_plug_vst3 base sdk sdk_common)
target_include_directories(playground_plug_vst3 PRIVATE ${CMAKE_SOURCE_DIR}/src/playground_plug ${CMAKE_SOURCE_DIR}/src/playground_plug_vst3)
target_include_directories(playground_plug_vst3 SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/vst3/base ${CMAKE_SOURCE_DIR}/lib/vst3/pluginterfaces ${CMAKE_SOURCE_DIR}/lib/vst3/public.sdk)
set_target_properties(playground_plug_vst3 PROPERTIES SUFFIX ".vst3")
set_target_properties(playground_plug_vst3 PROPERTIES OUTPUT_NAME "PlaygroundPlug")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${VST3_SRC_DIR} FILES ${VST3_SRC_FILES})
endif()

# vst3 to dist
add_custom_command(TARGET playground_plug_vst3 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:playground_plug_vst3>
  ${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/$<$<CONFIG:Debug>:Debug$<$<CONFIG:Release>:Release>/PlaygroundPlug.vst3/Contents/x86_64-win/PlaygroundPlug.vst3)
