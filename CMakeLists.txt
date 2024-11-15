# global
cmake_minimum_required(VERSION 3.21)

# needs to be before project
set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo CACHE TYPE INTERNAL FORCE)

# project
project(playground-plug)

# build
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15 CACHE STRING "Minimum macOS version")
set_property(GLOBAL PROPERTY USE_FOLDERS YES)

# name
set (PROJECT_NAME_IN playground_plug)
set (PROJECT_NAME_IN_VST3 "${PROJECT_NAME_IN}_vst3")
set (PROJECT_NAME_OUT PlaygroundPlug)
set (PROJECT_NAME_OUT_VST3 "${PROJECT_NAME_OUT}VST3")

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

# plugin
set (SRC_DIR "${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_IN}")
file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.*")
add_library(${PROJECT_NAME_IN} STATIC ${SRC_FILES})
target_include_directories(${PROJECT_NAME_IN} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_IN})
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${SRC_DIR} FILES ${SRC_FILES})
endif()

# plugin vst3
set (VST3_SRC_DIR "${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_IN_VST3}")
file(GLOB_RECURSE VST3_SRC_FILES "${VST3_SRC_DIR}/*.*")
add_library(${PROJECT_NAME_IN_VST3} SHARED ${VST3_SRC_FILES})
target_link_libraries(${PROJECT_NAME_IN_VST3} ${PROJECT_NAME_IN})
target_link_libraries(${PROJECT_NAME_IN_VST3} base sdk sdk_common)
target_include_directories(${PROJECT_NAME_IN_VST3} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_IN} ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_IN_VST3})
target_include_directories(${PROJECT_NAME_IN_VST3} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/vst3/base ${CMAKE_SOURCE_DIR}/lib/vst3/pluginterfaces ${CMAKE_SOURCE_DIR}/lib/vst3/public.sdk)
set_target_properties(${PROJECT_NAME_IN_VST3} PROPERTIES SUFFIX ".vst3")
set_target_properties(${PROJECT_NAME_IN_VST3} PROPERTIES OUTPUT_NAME ${PROJECT_NAME_OUT})
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${VST3_SRC_DIR} FILES ${VST3_SRC_FILES})
endif()

# vst3 to dist
add_custom_command(TARGET ${PROJECT_NAME_IN_VST3} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME_IN_VST3}>
  "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>/${PROJECT_NAME_OUT}.vst3/Contents/x86_64-win/${PROJECT_NAME_OUT}.vst3"
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:${PROJECT_NAME_IN_VST3}>/${PROJECT_NAME_OUT}.pdb
  "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>/${PROJECT_NAME_OUT}.vst3/Contents/x86_64-win/${PROJECT_NAME_OUT}.pdb")