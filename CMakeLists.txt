cmake_minimum_required(VERSION 3.9) # CMP0069 NEW
project(SLEDLIB)

cmake_policy(SET CMP0069 NEW)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_BINARY_DIR}/include)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(cotire)

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_STANDARD 17)
set(TARGET_ARCH "x64" CACHE STRING "Target Architecture")
set(SLED_X86_64 1 CACHE STRING "Targeting x86-64 Architecture")

#
# Cortire unity build and precompiled header support.
#
#include(cotire)
#set(COTIRE_ADD_UNITY_BUILD FALSE)
#set_directory_properties(PROPERTIES COTIRE_PREFIX_HEADER_INCLUDE_PATH "${COTIRE_PREFIX_HEADER_INCLUDE_PATH}; ${CMAKE_SOURCE_DIR}/3rdparty")
#set_directory_properties(PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)

# Build specific flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fstrict-vtable-pointers")

#
# Build Options
#
option(SLED_BUILD_UNIT_TESTS "Build unit tests" ON)
option(SLED_BUILD_TESTS "Build stand-alone tests" ON)
option(SLED_BUILD_GTEST "Build gtest" ON)
option(SLED_BUILD_DOCS "Build docs" ON)
option(SLED_BUILD_LUA "Build LUA" ON)

#
# Helper function to add unit test
#
function(add_unit_test)
    cmake_parse_arguments(
        PARSED_ARGS
        ""
        "NAME"
        "SRC;DEPS"
        ${ARGN})
    if(NOT PARSED_ARGS_NAME)
        message(FATAL_ERROR "You must provide a name to add_unit_test")
    endif(NOT PARSED_ARGS_NAME)
    if (SLED_BUILD_UNIT_TESTS)
        add_executable(
            ${PARSED_ARGS_NAME}
            ${PARSED_ARGS_SRC})
        target_link_libraries(${PARSED_ARGS_NAME}
            ${PARSED_ARGS_DEPS}
            gtest gtest_main)
        add_test(${PARSED_ARGS_NAME} ${PARSED_ARGS_NAME})
        set_target_properties(${PARSED_ARGS_NAME} PROPERTIES FOLDER Tests)
    endif (SLED_BUILD_UNIT_TESTS)
endfunction(add_unit_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -Wall -Werror -g -Wno-unused-private-field -Wuninitialized -O0")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2")

set(REBUILD_3RDPARTY ON CACHE BOOL "")

add_subdirectory(3rdparty)
include_directories(3rdparty)

add_subdirectory(include)
add_subdirectory(src)
if (SLED_BUILD_TESTS)
    add_subdirectory(test)
endif (SLED_BUILD_TESTS)
if (SLED_BUILD_DOCS)
    add_subdirectory(docs)
endif (SLED_BUILD_DOCS)
