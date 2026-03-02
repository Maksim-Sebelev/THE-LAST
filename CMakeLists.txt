cmake_minimum_required(VERSION 3.30)

# =================================================================================================

project(TheLast
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

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_SETTINGS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

string(TIMESTAMP CURRENT_TIME "%Y-%m-%d %H:%M:%S")

# =================================================================================================

include(${CMAKE_SETTINGS_DIR}/debug.cmake OPTIONAL)

# =================================================================================================
# crete paracl info library
set(THELAST_INFO_DIR ${SRC_DIR}/lib-info)
set(THELAST_INFO_SRC
    ${THELAST_INFO_DIR}/lib-info.cppm.in
)

set(THELAST_INFO_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib-info)
set(THELAST_INFO_BUILD_SRC ${THELAST_INFO_BUILD_DIR}/lib-info.cppm)

file(MAKE_DIRECTORY ${THELAST_INFO_BUILD_DIR})

# get git commit hash
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

if(NOT GIT_COMMIT_HASH)
    set(GIT_COMMIT_HASH "unknown")
endif()

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
    BASE_DIRS ${THELAST_INFO_BUILD_DIR}
    FILES ${THELAST_INFO_BUILD_SRC}
)

target_include_directories(${THELAST_INFO_LIB}
    PUBLIC
        $<BUILD_INTERFACE:${THELAST_INFO_BUILD_DIR}>
)

if(COMMAND target_hard_debug_options)
    target_hard_debug_options(${THELAST_INFO_LIB})
endif()

# =================================================================================================
# node lib

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
    BASE_DIRS ${SRC_DIR}/nodes
    FILES ${NODES_SRC}
)

target_include_directories(${NODES_LIB}
    PUBLIC
        $<BUILD_INTERFACE:${SRC_DIR}/nodes>
)

if(COMMAND target_hard_debug_options)
    target_hard_debug_options(${NODES_LIB})
endif()
if(COMMAND target_hard_debug_sanitizers)
    target_hard_debug_sanitizers(${NODES_LIB})
endif()

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
    BASE_DIRS ${SRC_DIR}/ast
    FILES ${AST_SRC}
)

target_link_libraries(${AST_LIB}
  PUBLIC
    ${NODES_LIB}
)

target_include_directories(${AST_LIB}
    PUBLIC
        $<BUILD_INTERFACE:${SRC_DIR}/ast>
)

if(COMMAND target_hard_debug_options)
    target_hard_debug_options(${AST_LIB})
endif()
if(COMMAND target_hard_debug_sanitizers)
    target_hard_debug_sanitizers(${AST_LIB})
endif()

# =================================================================================================
# ast functional

set(AST_FUNCTIONAL_LIB ast-functional)
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
    BASE_DIRS ${SRC_DIR}/functional
    FILES ${AST_FUNCTIONAL_SRC}
)

target_link_libraries(${AST_FUNCTIONAL_LIB}
  PUBLIC
    ${NODES_LIB}
    ${AST_LIB}
    ${THELAST_INFO_LIB}
)

target_include_directories(${AST_FUNCTIONAL_LIB}
    PUBLIC
        $<BUILD_INTERFACE:${SRC_DIR}/functional>
)

if(COMMAND target_hard_debug_options)
    target_hard_debug_options(${AST_FUNCTIONAL_LIB})
endif()
if(COMMAND target_hard_debug_sanitizers)
    target_hard_debug_sanitizers(${AST_FUNCTIONAL_LIB})
endif()

# =================================================================================================

set(THELAST_LIB TheLast)
add_library(${THELAST_LIB})

target_sources(${THELAST_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    BASE_DIRS ${SRC_DIR}
    FILES ${SRC_DIR}/thelast.cppm
)

target_link_libraries(${THELAST_LIB}
    PUBLIC
        ${THELAST_INFO_LIB}
        ${AST_LIB}
        ${AST_FUNCTIONAL_LIB}
        ${NODES_LIB}
)

target_include_directories(${THELAST_LIB}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# =================================================================================================
# create ALIAS targets with :: for external use

set(THELAST_NAMESPACE TheLast)

add_library(${THELAST_NAMESPACE}::lib-info ALIAS ${THELAST_INFO_LIB})
add_library(${THELAST_NAMESPACE}::nodes ALIAS ${NODES_LIB})
add_library(${THELAST_NAMESPACE}::ast ALIAS ${AST_LIB})
add_library(${THELAST_NAMESPACE}::ast-functional ALIAS ${AST_FUNCTIONAL_LIB})
add_library(${THELAST_NAMESPACE}::TheLast ALIAS ${THELAST_LIB})

# =================================================================================================
# make test optional

option(THE-LAST_BUILD_TESTS "Build tests" OFF)

if(THE-LAST_BUILD_TESTS)
    add_executable(test-the-last
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp
    )

    target_link_libraries(test-the-last
        PRIVATE
            ${THELAST_LIB}
    )

    target_include_directories(test-the-last
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endif()

# =================================================================================================
# install and export

include(GNUInstallDirs)

install(TARGETS
        ${THELAST_INFO_LIB}
        ${NODES_LIB}
        ${AST_LIB}
        ${AST_FUNCTIONAL_LIB}
        ${THELAST_LIB}
    EXPORT THE-LASTTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/modules
)

install(EXPORT THE-LASTTargets
    FILE THE-LASTTargets.cmake
    NAMESPACE ${THELAST_NAMESPACE}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/THE-LAST
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/THE-LASTConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/THE-LASTConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/THE-LAST
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/THE-LASTConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/THE-LAST
)

# =================================================================================================