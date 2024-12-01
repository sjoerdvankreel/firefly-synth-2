# global
cmake_minimum_required(VERSION 3.21)
project(playground_plug)

# build
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15 CACHE STRING "Minimum macOS version")
set_property(GLOBAL PROPERTY USE_FOLDERS YES)

# project
set (PROJECT_VERSION_MAJOR 3)
set (PROJECT_VERSION_MINOR 0)
set (PROJECT_VERSION_PATCH 0)
set (PROJECT_NAME_PLUG playground_plug)
set (PROJECT_NAME_BASE playground_base)
set (PROJECT_NAME_PLUG_VST3 "${PROJECT_NAME_PLUG}_vst3")
set (PROJECT_NAME_BASE_VST3 "${PROJECT_NAME_BASE}_vst3")
set (PROJECT_NAME_OUT PlaygroundPlug)

# juce
set(JUCE_MODULES_DIR "${CMAKE_SOURCE_DIR}/lib/JUCE/modules")
add_library(juce STATIC 
  "${JUCE_MODULES_DIR}/juce_core/juce_core.cpp"
  "${JUCE_MODULES_DIR}/juce_core/juce_core_CompilationTime.cpp")
target_include_directories(juce PRIVATE "${JUCE_MODULES_DIR}")
target_compile_definitions(juce PRIVATE JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)

# vst3 sdk
set(VST_SDK TRUE)
set(SMTG_ENABLE_VSTGUI_SUPPORT OFF)
set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF)
set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF)
set(public_sdk_SOURCE_DIR "${CMAKE_SOURCE_DIR}/lib/vst3/public.sdk")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/lib/vst3/cmake/modules")
include(SMTG_VST3_SDK)
smtg_setup_platform_toolset()
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/vst3/base")
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/vst3/public.sdk")
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/vst3/pluginterfaces")

# plugin base
set (BASE_SRC_DIR "${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE}")
file(GLOB_RECURSE BASE_SRC_FILES "${BASE_SRC_DIR}/*.*")
add_library(${PROJECT_NAME_BASE} STATIC ${BASE_SRC_FILES})
target_include_directories(${PROJECT_NAME_BASE} SYSTEM PRIVATE "${JUCE_MODULES_DIR}")
target_include_directories(${PROJECT_NAME_BASE} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE})
target_compile_definitions(${PROJECT_NAME_BASE} PRIVATE JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${BASE_SRC_DIR} FILES ${BASE_SRC_FILES})
endif()

# plugin plug
set (PLUG_SRC_DIR "${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_PLUG}")
file(GLOB_RECURSE PLUG_SRC_FILES "${PLUG_SRC_DIR}/*.*")
add_library(${PROJECT_NAME_PLUG} STATIC ${PLUG_SRC_FILES})
target_include_directories(${PROJECT_NAME_PLUG} SYSTEM PRIVATE "${JUCE_MODULES_DIR}")
target_include_directories(${PROJECT_NAME_PLUG} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_PLUG})
target_include_directories(${PROJECT_NAME_PLUG} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE})
target_compile_definitions(${PROJECT_NAME_PLUG} PRIVATE JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)
target_compile_definitions(${PROJECT_NAME_PLUG} PRIVATE FF_PLUG_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
target_compile_definitions(${PROJECT_NAME_PLUG} PRIVATE FF_PLUG_VERSION_MINOR=${PROJECT_VERSION_MINOR})
target_compile_definitions(${PROJECT_NAME_PLUG} PRIVATE FF_PLUG_VERSION_PATCH=${PROJECT_VERSION_PATCH})
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${PLUG_SRC_DIR} FILES ${PLUG_SRC_FILES})
endif()

# vst3 base
set (BASE_VST3_SRC_DIR "${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE_VST3}")
file(GLOB_RECURSE BASE_VST3_SRC_FILES "${BASE_VST3_SRC_DIR}/*.*")
add_library(${PROJECT_NAME_BASE_VST3} STATIC ${BASE_VST3_SRC_FILES})
target_include_directories(${PROJECT_NAME_BASE_VST3} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/vst3)
target_include_directories(${PROJECT_NAME_BASE_VST3} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE})
target_include_directories(${PROJECT_NAME_BASE_VST3} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE_VST3})
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${BASE_VST3_SRC_DIR} FILES ${BASE_VST3_SRC_FILES})
endif()

# vst3 plug
set (PLUG_VST3_SRC_DIR "${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_PLUG_VST3}")
file(GLOB_RECURSE PLUG_VST3_SRC_FILES "${PLUG_VST3_SRC_DIR}/*.*")
add_library(${PROJECT_NAME_PLUG_VST3} SHARED ${PLUG_VST3_SRC_FILES})
smtg_target_add_library_main(${PROJECT_NAME_PLUG_VST3})
target_link_libraries(${PROJECT_NAME_PLUG_VST3} ${PROJECT_NAME_BASE})
target_link_libraries(${PROJECT_NAME_PLUG_VST3} ${PROJECT_NAME_PLUG})
target_link_libraries(${PROJECT_NAME_PLUG_VST3} ${PROJECT_NAME_BASE_VST3})
target_link_libraries(${PROJECT_NAME_PLUG_VST3} juce base sdk sdk_common)
target_include_directories(${PROJECT_NAME_PLUG_VST3} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/vst3)
target_include_directories(${PROJECT_NAME_PLUG_VST3} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE})
target_include_directories(${PROJECT_NAME_PLUG_VST3} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_PLUG})
target_include_directories(${PROJECT_NAME_PLUG_VST3} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE_VST3})
target_include_directories(${PROJECT_NAME_PLUG_VST3} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_PLUG_VST3})
set_target_properties(${PROJECT_NAME_PLUG_VST3} PROPERTIES OUTPUT_NAME ${PROJECT_NAME_OUT})
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  set_target_properties(${PROJECT_NAME_PLUG_VST3} PROPERTIES SUFFIX ".vst3")
endif ()
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${PLUG_VST3_SRC_DIR} FILES ${PLUG_VST3_SRC_FILES})
endif()

# vst3 to dist
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_VST3} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME_PLUG_VST3}>
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>/${PROJECT_NAME_OUT}.vst3/Contents/x86_64-win/${PROJECT_NAME_OUT}.vst3"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:${PROJECT_NAME_PLUG_VST3}>/${PROJECT_NAME_OUT}.pdb
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>/${PROJECT_NAME_OUT}.vst3/Contents/x86_64-win/${PROJECT_NAME_OUT}.pdb" || (exit 0))
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_VST3} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME_PLUG_VST3}>
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME_OUT}.vst3/Contents/x86_64-linux/${PROJECT_NAME_OUT}.so")
else ()
  message(FATAL_ERROR "Unsupported platform.")
endif()