cmake_minimum_required(VERSION 2.8)

project(hello_world_gles)

file(GLOB sourceFiles ../../hello_world_gles.c)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	SET(linkFlags "")
else() # Either MinSizeRel, RelWithDebInfo or Release, all which run with optimizations enabled.
	SET(linkFlags "-O2")
endif()

if (NOT CMAKE_AR OR NOT EXISTS "${CMAKE_AR}")
	message(FATAL_ERROR "CMAKE_AR='${CMAKE_AR}' does not exist for Emscripten toolchain!")
endif()

if (NOT CMAKE_RANLIB OR NOT EXISTS "${CMAKE_RANLIB}")
	message(FATAL_ERROR "CMAKE_RANLIB='${CMAKE_RANLIB}' does not exist for Emscripten toolchain!")
endif()

if (NOT CMAKE_C_COMPILER OR NOT EXISTS "${CMAKE_C_COMPILER}")
	message(FATAL_ERROR "CMAKE_C_COMPILER='${CMAKE_C_COMPILER}' does not exist for Emscripten toolchain!")
endif()

if (NOT CMAKE_CXX_COMPILER OR NOT EXISTS "${CMAKE_CXX_COMPILER}")
	message(FATAL_ERROR "CMAKE_CXX_COMPILER='${CMAKE_CXX_COMPILER}' does not exist for Emscripten toolchain!")
endif()

if (WIN32)
	message(FATAL_ERROR "WIN32 should not be defined when cross-compiling!")
endif()

if (APPLE)
	message(FATAL_ERROR "APPLE should not be defined when cross-compiling!")
endif()

if (NOT EMSCRIPTEN)
	message(FATAL_ERROR "EMSCRIPTEN should be defined when cross-compiling!")
endif()

if (NOT CMAKE_C_SIZEOF_DATA_PTR)
	message(FATAL_ERROR "CMAKE_C_SIZEOF_DATA_PTR was not defined!")
endif()

# with NO_DEFAULT_PATH + ONLY_CMAKE_FIND_ROOT_PATH, we ensure that
# only ${CMAKE_FIND_ROOT_PATH}/include will be searched for
# emscripten/emscripten.h
find_path(EMSCRIPTEN_INCLUDE_DIR emscripten/emscripten.h
	PATHS /include
	NO_DEFAULT_PATH
	ONLY_CMAKE_FIND_ROOT_PATH
	)

if(NOT EMSCRIPTEN_INCLUDE_DIR)
	message(FATAL_ERROR "emscripten.h could not be found! Is CMAKE_FIND_ROOT_PATH='${CMAKE_FIND_ROOT_PATH}' correct?")
endif()

include(CheckIncludeFile)

check_include_file(stdlib.h HAVE_STDLIB_H)
if (NOT ${HAVE_STDLIB_H})
	message(FATAL_ERROR "CMake script check_include_file failed! Could not find stdlib.h via it!")
endif()

if (NOT CMAKE_EXECUTABLE_SUFFIX STREQUAL ".js")
	message(FATAL_ERROR "The default suffix for building executables should be .js!")
endif()
SET(CMAKE_EXECUTABLE_SUFFIX ".html")

# CMake built-in check_include_file() macro interacts with CMAKE_EXECUTABLE_SUFFIX, so make sure it works after user changes that.

check_include_file(math.h HAVE_MATH_H)
if (NOT ${HAVE_MATH_H})
	message(FATAL_ERROR "CMake script check_include_file failed! Could not find math.h via it!")
endif()

add_executable(hello_world_gles ${sourceFiles})
set_target_properties(hello_world_gles PROPERTIES LINK_FLAGS "${linkFlags}")

# Validating asm.js requires SpiderMonkey JS VM - detect its presence via the SPIDERMONKEY environment variable.
if (DEFINED ENV{SPIDERMONKEY} AND CMAKE_BUILD_TYPE STREQUAL Release)
	em_validate_asmjs_after_build(hello_world_gles)
endif()
