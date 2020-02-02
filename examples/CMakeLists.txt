cmake_minimum_required (VERSION 2.6)

set (MODULE_NAME clips_examples)
project (${MODULE_NAME})

#------------------------------------------------------------------------------
# examples

# message
message ("${MODULE_NAME} for ${CMAKE_BUILD_TYPE}")

# include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../examples)

# src
file(GLOB_RECURSE HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../examples/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../examples/*.hpp"
)
source_group("includes" FILES ${HEADERS})
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../examples SRCS)

# target
add_executable(${MODULE_NAME} ${SRCS} ${HEADERS})

#install
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin)

#------------------------------------------------------------------------------
