cmake_minimum_required(VERSION 3.30)

# =================================================================================================

project(THE-LAST
    LANGUAGES CXX
    VERSION 1.0
)
# =================================================================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# =================================================================================================
# set some global varibles

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(CMAKE_SETTINGS_DIR ${PROJECT_SOURCE_DIR}/cmake)

string(TIMESTAMP CURRENT_TIME "%Y-%m-%d %H:%M:%S")

# =================================================================================================

include(${CMAKE_SETTINGS_DIR}/debug.cmake)

# =================================================================================================
# crete paracl info library
set(THELAST_INFO_DIR ${SRC_DIR}/lib-info)
set(THELAST_INFO_SRC
    ${THELAST_INFO_DIR}/lib-info.cppm.in
)

set(THELAST_INFO_BUILD_DIR ${CMAKE_BINARY_DIR}/lib-info)
set(THELAST_INFO_BUILD_SRC ${THELAST_INFO_BUILD_DIR}/lib-info.cppm)

# get git commit hash
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# create module with paracl info
configure_file(
    ${THELAST_INFO_SRC}
    ${THELAST_INFO_BUILD_SRC}
    @ONLY
)

set(THELAST_INFO_LIB lib-info)
add_library(${THELAST_INFO_LIB})

target_sources(${THELAST_INFO_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${THELAST_INFO_BUILD_SRC}
)

target_hard_debug_options(${THELAST_INFO_LIB})

# =================================================================================================
# node lib (ast = abstract syntax tree)

set(NODES_LIB nodes)
add_library(${NODES_LIB})

set(NODES_SRC_DIR ${SRC_DIR}/nodes)
set(NODES_SRC
    ${NODES_SRC_DIR}/nodes.cppm
    ${NODES_SRC_DIR}/nodes-type-erasure.cppm
    ${NODES_SRC_DIR}/nodes-traits.cppm
)

target_sources(${NODES_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${NODES_SRC}
)

target_hard_debug_options(${NODES_LIB})
target_hard_debug_sanitizers(${NODES_LIB})

# =================================================================================================
# ast lib (ast = abstract syntax tree)

set(AST_LIB ast)
add_library(${AST_LIB})

set(AST_SRC_DIR ${SRC_DIR}/ast)
set(AST_SRC
    ${AST_SRC_DIR}/ast.cppm
)

target_sources(${AST_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${AST_SRC}
)

target_link_libraries(${AST_LIB}
  PUBLIC
    ${NODES_LIB}
)

target_hard_debug_options(${AST_LIB})
target_hard_debug_sanitizers(${AST_LIB})

# =================================================================================================
# ast functional

set(AST_FUNCTIONAL_LIB ast-functoinal)
add_library(${AST_FUNCTIONAL_LIB})

set(AST_FUNCTIONAL_SRC_DIR ${SRC_DIR}/functional)
set(AST_FUNCTIONAL_SRC
    ${AST_FUNCTIONAL_SRC_DIR}/write.cppm
    ${AST_FUNCTIONAL_SRC_DIR}/read.cppm
    ${AST_FUNCTIONAL_SRC_DIR}/graphic-dump.cppm
)

target_sources(${AST_FUNCTIONAL_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${AST_FUNCTIONAL_SRC}
)

target_link_libraries(${AST_FUNCTIONAL_LIB}
  PUBLIC
    ${NODES_LIB}
    ${AST_LIB}
    ${THELAST_INFO_LIB}
)

target_hard_debug_options(${AST_FUNCTIONAL_LIB})
target_hard_debug_sanitizers(${AST_FUNCTIONAL_LIB})

# =================================================================================================

set(THELAST_LIB TheLast)

add_library(${THELAST_LIB})

target_sources(${THELAST_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${SRC_DIR}/thelast.cppm
)

target_link_libraries(${THELAST_LIB}
    PUBLIC
        ${THELAST_INFO_LIB}
        ${AST_LIB}
        ${AST_FUNCTIONAL_LIB}
        ${NODES_LIB}
)

# =================================================================================================

add_executable(test
${SRC_DIR}/../tests/test.cpp
)

target_link_libraries(test
PRIVATE
    ${THELAST_LIB}
)

# =================================================================================================
