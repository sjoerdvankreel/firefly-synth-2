# global
cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0092 NEW)
project(firefly_synth_2)

# build
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "" FORCE)
set (CMAKE_OSX_DEPLOYMENT_TARGET 10.15 CACHE STRING "Minimum macOS version")
set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set (CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
set_property(GLOBAL PROPERTY USE_FOLDERS YES)

# project
set (PROJECT_VERSION_MAJOR 2)
set (PROJECT_VERSION_MINOR 0)
set (PROJECT_VERSION_PATCH 5)
set (PROJECT_NAME_BASE firefly_base)
set (PROJECT_NAME_PLUG firefly_synth)
set (PROJECT_NAME_BASE_JUCE firefly_base_juce)
set (PROJECT_NAME_PLUG_VST3 "${PROJECT_NAME_PLUG}_vst3")
set (PROJECT_NAME_BASE_VST3 "${PROJECT_NAME_BASE}_vst3")
set (PROJECT_NAME_PLUG_CLAP "${PROJECT_NAME_PLUG}_clap")
set (PROJECT_NAME_BASE_CLAP "${PROJECT_NAME_BASE}_clap")
set (PROJECT_NAME_OUT FireflySynth2)

# want this only for dev
if (NOT DEFINED FB_WARN_AS_ERROR)
  message(FATAL_ERROR "FB_WARN_AS_ERROR is not set.")
endif()
if (NOT DEFINED FB_ENABLE_ASAN)
  message(FATAL_ERROR "FB_ENABLE_ASAN is not set.")
endif()

# stuff that should apply to all code
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG=1 /Zi")
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
  set (CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG")
  if(FB_ENABLE_ASAN)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /fsanitize=address")
    set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /fsanitize=address")
  endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG=1")
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
  if(FB_ENABLE_ASAN)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
    set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fsanitize=address")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG=1")
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto=auto")
  if(FB_ENABLE_ASAN)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
    set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fsanitize=address")
  endif()
else ()
  message (FATAL_ERROR "Unsupported compiler.")
endif ()

# stuff that should only apply to our code
# note: for GCC we can't turn on warnings as errors for the final .SO's because juce produces some,
# and it's compiled with the dynamic library projects (i tried to factor out juce to separate project, but crash and burn on Linux)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set (FF_COMPILE_OPTIONS_DYN /W4 /w45038 /w45246 /w45266 /w45233 /analyze:external- /fp:fast /utf-8)
  if(FB_WARN_AS_ERROR)
    set (FF_COMPILE_OPTIONS_DYN ${FF_COMPILE_OPTIONS_DYN} /WX)
  endif()
  set (FF_COMPILE_OPTIONS_LIB "${FF_COMPILE_OPTIONS_DYN}")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set (FF_COMPILE_OPTIONS_DYN -Wall -Wextra -pedantic -Wno-sign-compare -Wno-parentheses -Wno-braced-scalar-init -ffast-math -fvisibility=hidden -fvisibility-inlines-hidden)
  if(FB_WARN_AS_ERROR)
    set (FF_COMPILE_OPTIONS_DYN ${FF_COMPILE_OPTIONS_DYN} -Werror)
  endif()
  set (FF_COMPILE_OPTIONS_LIB "${FF_COMPILE_OPTIONS_DYN}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set (FF_COMPILE_OPTIONS_DYN -Wall -Wextra -pedantic -Wno-sign-compare -Wno-parentheses -Wno-class-memaccess -ffast-math -Wl,--no-undefined -fvisibility=hidden -fvisibility-inlines-hidden)
  set (FF_COMPILE_OPTIONS_LIB -Wall -Wextra -pedantic -Wno-sign-compare -Wno-parentheses -Wno-class-memaccess -ffast-math -Wl,--no-undefined -fvisibility=hidden -fvisibility-inlines-hidden)
  if(FB_WARN_AS_ERROR)
    set (FF_COMPILE_OPTIONS_LIB ${FF_COMPILE_OPTIONS_LIB} -Werror)
  endif()
else ()
  message (FATAL_ERROR "Unsupported compiler.")
endif ()

# juce
set(JUCE_MODULES_ONLY ON)
add_definitions(-DJUCE_USE_CURL=0)
add_definitions(-DJUCE_MODAL_LOOPS_PERMITTED=0)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/JUCE)

# mts-esp
set (MTS_ESP_SRC_DIR "${CMAKE_SOURCE_DIR}/lib/MTS-ESP/Client")
file(GLOB_RECURSE MTS_ESP_SRC_FILES "${MTS_ESP_SRC_DIR}/*.*")
add_library(MTS-ESP STATIC ${MTS_ESP_SRC_FILES})
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${MTS_ESP_SRC_DIR} FILES ${MTS_ESP_SRC_FILES})
endif()

# vst3 sdk
set(VST_SDK TRUE)
set(SMTG_USE_STATIC_CRT ON)
set(SMTG_ENABLE_VSTGUI_SUPPORT OFF)
set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF)
set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF)
if(FB_ENABLE_ASAN)
  set(SMTG_ENABLE_ADDRESS_SANITIZER ON)
endif()
set(public_sdk_SOURCE_DIR "${CMAKE_SOURCE_DIR}/lib/vst3/public.sdk")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/lib/vst3/cmake/modules")
include(SMTG_VST3_SDK)
smtg_setup_platform_toolset()
# vst3 cmake changes this
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options($<$<CONFIG:Debug>:/Zi>)
endif()
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo")
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/vst3/base")
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/vst3/public.sdk")
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/vst3/pluginterfaces")

# plugin base
set (BASE_SRC_DIR "${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE}")
file(GLOB_RECURSE BASE_SRC_FILES "${BASE_SRC_DIR}/*.*")
add_library(${PROJECT_NAME_BASE} STATIC ${BASE_SRC_FILES})
target_compile_options(${PROJECT_NAME_BASE} PRIVATE "${FF_COMPILE_OPTIONS_LIB}")
target_include_directories(${PROJECT_NAME_BASE} SYSTEM PRIVATE ${JUCE_MODULES_DIR})
target_include_directories(${PROJECT_NAME_BASE} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/sse2neon)
target_include_directories(${PROJECT_NAME_BASE} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/xsimd/include)
target_include_directories(${PROJECT_NAME_BASE} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/MTS-ESP/Client)
target_include_directories(${PROJECT_NAME_BASE} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE})
target_compile_definitions(${PROJECT_NAME_BASE} PRIVATE JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${BASE_SRC_DIR} FILES ${BASE_SRC_FILES})
endif()

# vst3 base
set (BASE_VST3_SRC_DIR "${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE_VST3}")
file(GLOB_RECURSE BASE_VST3_SRC_FILES "${BASE_VST3_SRC_DIR}/*.*")
add_library(${PROJECT_NAME_BASE_VST3} STATIC ${BASE_VST3_SRC_FILES})
target_compile_options(${PROJECT_NAME_BASE_VST3} PRIVATE "${FF_COMPILE_OPTIONS_LIB}")
target_include_directories(${PROJECT_NAME_BASE_VST3} SYSTEM PRIVATE ${JUCE_MODULES_DIR})
target_include_directories(${PROJECT_NAME_BASE_VST3} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/vst3)
target_include_directories(${PROJECT_NAME_BASE_VST3} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/xsimd/include)
target_include_directories(${PROJECT_NAME_BASE_VST3} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE})
target_include_directories(${PROJECT_NAME_BASE_VST3} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE_VST3})
target_compile_definitions(${PROJECT_NAME_BASE_VST3} PRIVATE JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${BASE_VST3_SRC_DIR} FILES ${BASE_VST3_SRC_FILES})
endif()

# clap base
set (BASE_CLAP_SRC_DIR "${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE_CLAP}")
file(GLOB_RECURSE BASE_CLAP_SRC_FILES "${BASE_CLAP_SRC_DIR}/*.*")
add_library(${PROJECT_NAME_BASE_CLAP} STATIC ${BASE_CLAP_SRC_FILES})
target_compile_options(${PROJECT_NAME_BASE_CLAP} PRIVATE "${FF_COMPILE_OPTIONS_LIB}")
target_include_directories(${PROJECT_NAME_BASE_CLAP} SYSTEM PRIVATE ${JUCE_MODULES_DIR})
target_include_directories(${PROJECT_NAME_BASE_CLAP} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/clap/include)
target_include_directories(${PROJECT_NAME_BASE_CLAP} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/clap-helpers/include)
target_include_directories(${PROJECT_NAME_BASE_CLAP} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/xsimd/include)
target_include_directories(${PROJECT_NAME_BASE_CLAP} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/readerwriterqueue)
target_include_directories(${PROJECT_NAME_BASE_CLAP} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE})
target_include_directories(${PROJECT_NAME_BASE_CLAP} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE_CLAP})
target_compile_definitions(${PROJECT_NAME_BASE_CLAP} PRIVATE JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${BASE_CLAP_SRC_DIR} FILES ${BASE_CLAP_SRC_FILES})
endif()

# plugin plug
set (PLUG_SRC_DIR "${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_PLUG}")
file(GLOB_RECURSE PLUG_SRC_FILES "${PLUG_SRC_DIR}/*.*")
add_library(${PROJECT_NAME_PLUG} STATIC ${PLUG_SRC_FILES})
target_compile_options(${PROJECT_NAME_PLUG} PRIVATE "${FF_COMPILE_OPTIONS_LIB}")
target_include_directories(${PROJECT_NAME_PLUG} SYSTEM PRIVATE ${JUCE_MODULES_DIR})
target_include_directories(${PROJECT_NAME_PLUG} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/xsimd/include)
target_include_directories(${PROJECT_NAME_PLUG} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/MTS-ESP/Client)
target_include_directories(${PROJECT_NAME_PLUG} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_PLUG})
target_include_directories(${PROJECT_NAME_PLUG} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE})
target_compile_definitions(${PROJECT_NAME_PLUG} PRIVATE JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)
target_compile_definitions(${PROJECT_NAME_PLUG} PUBLIC FF_PLUG_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
target_compile_definitions(${PROJECT_NAME_PLUG} PUBLIC FF_PLUG_VERSION_MINOR=${PROJECT_VERSION_MINOR})
target_compile_definitions(${PROJECT_NAME_PLUG} PUBLIC FF_PLUG_VERSION_PATCH=${PROJECT_VERSION_PATCH})
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${PLUG_SRC_DIR} FILES ${PLUG_SRC_FILES})
endif()

# vst3 plug
set (PLUG_VST3_SRC_DIR "${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_PLUG_VST3}")
file(GLOB_RECURSE PLUG_VST3_SRC_FILES "${PLUG_VST3_SRC_DIR}/*.*")
add_library(${PROJECT_NAME_PLUG_VST3} SHARED ${PLUG_VST3_SRC_FILES})
target_compile_options(${PROJECT_NAME_PLUG_VST3} PRIVATE "${FF_COMPILE_OPTIONS_DYN}")
smtg_target_add_library_main(${PROJECT_NAME_PLUG_VST3})
target_link_libraries(${PROJECT_NAME_PLUG_VST3} PRIVATE ${PROJECT_NAME_BASE_VST3})
target_link_libraries(${PROJECT_NAME_PLUG_VST3} PRIVATE ${PROJECT_NAME_PLUG})
target_link_libraries(${PROJECT_NAME_PLUG_VST3} PRIVATE ${PROJECT_NAME_BASE})
target_link_libraries(${PROJECT_NAME_PLUG_VST3} PRIVATE juce_core juce_events juce_gui_basics juce_dsp base sdk sdk_common MTS-ESP)
target_include_directories(${PROJECT_NAME_PLUG_VST3} SYSTEM PRIVATE ${JUCE_MODULES_DIR})
target_include_directories(${PROJECT_NAME_PLUG_VST3} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/vst3)
target_include_directories(${PROJECT_NAME_PLUG_VST3} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/xsimd/include)
target_include_directories(${PROJECT_NAME_PLUG_VST3} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE})
target_include_directories(${PROJECT_NAME_PLUG_VST3} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_PLUG})
target_include_directories(${PROJECT_NAME_PLUG_VST3} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE_VST3})
target_include_directories(${PROJECT_NAME_PLUG_VST3} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_PLUG_VST3})
target_compile_definitions(${PROJECT_NAME_PLUG_VST3} PRIVATE JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)
set_target_properties(${PROJECT_NAME_PLUG_VST3} PROPERTIES OUTPUT_NAME ${PROJECT_NAME_OUT})
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${PLUG_VST3_SRC_DIR} FILES ${PLUG_VST3_SRC_FILES})
endif()
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  set_target_properties(${PROJECT_NAME_PLUG_VST3} PROPERTIES SUFFIX ".vst3")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  set_target_properties(${PROJECT_NAME_PLUG_VST3} PROPERTIES PREFIX "" SUFFIX "")
endif ()

# clap plug
set (PLUG_CLAP_SRC_DIR "${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_PLUG_CLAP}")
file(GLOB_RECURSE PLUG_CLAP_SRC_FILES "${PLUG_CLAP_SRC_DIR}/*.*")
add_library(${PROJECT_NAME_PLUG_CLAP} SHARED ${PLUG_CLAP_SRC_FILES})
target_compile_options(${PROJECT_NAME_PLUG_CLAP} PRIVATE "${FF_COMPILE_OPTIONS_DYN}")
target_link_libraries(${PROJECT_NAME_PLUG_CLAP} PRIVATE ${PROJECT_NAME_BASE_CLAP})
target_link_libraries(${PROJECT_NAME_PLUG_CLAP} PRIVATE ${PROJECT_NAME_PLUG})
target_link_libraries(${PROJECT_NAME_PLUG_CLAP} PRIVATE ${PROJECT_NAME_BASE})
target_link_libraries(${PROJECT_NAME_PLUG_CLAP} PRIVATE juce_core juce_events juce_gui_basics juce_dsp MTS-ESP)
target_include_directories(${PROJECT_NAME_PLUG_CLAP} SYSTEM PRIVATE ${JUCE_MODULES_DIR})
target_include_directories(${PROJECT_NAME_PLUG_CLAP} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/xsimd/include)
target_include_directories(${PROJECT_NAME_PLUG_CLAP} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/clap/include)
target_include_directories(${PROJECT_NAME_PLUG_CLAP} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/clap-helpers/include)
target_include_directories(${PROJECT_NAME_PLUG_CLAP} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE})
target_include_directories(${PROJECT_NAME_PLUG_CLAP} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_PLUG})
target_include_directories(${PROJECT_NAME_PLUG_CLAP} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_BASE_CLAP})
target_include_directories(${PROJECT_NAME_PLUG_CLAP} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME_PLUG_CLAP})
target_include_directories(${PROJECT_NAME_PLUG_CLAP} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/lib/readerwriterqueue)
target_compile_definitions(${PROJECT_NAME_PLUG_CLAP} PRIVATE JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)
set_target_properties(${PROJECT_NAME_PLUG_CLAP} PROPERTIES OUTPUT_NAME ${PROJECT_NAME_OUT})
if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  set_target_properties(${PROJECT_NAME_PLUG_CLAP} PROPERTIES PREFIX "" SUFFIX "")
else ()
  set_target_properties(${PROJECT_NAME_PLUG_CLAP} PROPERTIES SUFFIX ".clap")
endif ()
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  source_group(TREE ${PLUG_CLAP_SRC_DIR} FILES ${PLUG_CLAP_SRC_FILES})
endif()

# vst3 to dist
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_VST3} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>/${PROJECT_NAME_OUT}.vst3/Contents/Resources")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_VST3} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME_PLUG_VST3}>
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>/${PROJECT_NAME_OUT}.vst3/Contents/x86_64-win/${PROJECT_NAME_OUT}.vst3"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:${PROJECT_NAME_PLUG_VST3}>/${PROJECT_NAME_OUT}.pdb
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>/${PROJECT_NAME_OUT}.vst3/Contents/x86_64-win/${PROJECT_NAME_OUT}.pdb" || (exit 0))
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_VST3} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME_PLUG_VST3}>
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME_OUT}.vst3/Contents/x86_64-linux/${PROJECT_NAME_OUT}.so")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_VST3} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME_OUT}.vst3/Contents/Resources")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_VST3} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME_PLUG_VST3}>
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME_OUT}.vst3/Contents/MacOS/${PROJECT_NAME_OUT}")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_CLAP} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/macos
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME_OUT}.vst3/Contents")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_VST3} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME_OUT}.vst3/Contents/Resources")
else ()
  message(FATAL_ERROR "Unsupported platform.")
endif()

# clap to dist
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_CLAP} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>/${PROJECT_NAME_OUT}.clap/Contents/Resources")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_CLAP} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME_PLUG_CLAP}>
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>/${PROJECT_NAME_OUT}.clap/Contents/x86_64-win/${PROJECT_NAME_OUT}.clap"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:${PROJECT_NAME_PLUG_CLAP}>/${PROJECT_NAME_OUT}.pdb
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>/${PROJECT_NAME_OUT}.clap/Contents/x86_64-win/${PROJECT_NAME_OUT}.pdb" || (exit 0))
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_CLAP} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME_PLUG_CLAP}>
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME_OUT}.clap/Contents/x86_64-linux/${PROJECT_NAME_OUT}.clap")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_CLAP} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME_OUT}.clap/Contents/Resources")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_CLAP} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME_PLUG_CLAP}>
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME_OUT}.clap/Contents/MacOS/${PROJECT_NAME_OUT}")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_CLAP} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/macos
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME_OUT}.clap/Contents")
  add_custom_command(TARGET ${PROJECT_NAME_PLUG_CLAP} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources
    "${CMAKE_SOURCE_DIR}/dist/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME_OUT}.clap/Contents/Resources")
else ()
  message(FATAL_ERROR "Unsupported platform.")
endif()
