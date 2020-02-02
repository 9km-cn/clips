cmake_minimum_required (VERSION 2.6)

set (MODULE_NAME clips_test)
project (${MODULE_NAME})

#------------------------------------------------------------------------------
# testing

# message
message ("${MODULE_NAME} for ${CMAKE_BUILD_TYPE}")

# include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../test)

# src
file(GLOB_RECURSE TEST_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../test/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../test/*.hpp"
)
source_group("includes" FILES ${TEST_HEADERS})
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../test TEST_SRCS)

# target
add_executable(${MODULE_NAME} ${TEST_SRCS} ${TEST_HEADERS})

#install
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin)

#------------------------------------------------------------------------------
