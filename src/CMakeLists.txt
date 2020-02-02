cmake_minimum_required (VERSION 2.6)

set (MODULE_NAME clips)
project (${MODULE_NAME})

# message
message ("${MODULE_NAME} for ${CMAKE_BUILD_TYPE}")

# def
if (NOT WIN32)
set (CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g3 -ggdb")
set (CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3")
set (CMAKE_CXX_FLAGS "-fPIC -m64 -Wall -std=c++11 ${CMAKE_CXX_FLAGS}")
endif ()
#add_definitions(-DEXPOERTS)

#------------------------------------------------------------------------------
# module

# include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# src
file(GLOB_RECURSE HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.hpp"
)
source_group("includes" FILES ${HEADERS})
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../src SRCS)

# target
add_library(${MODULE_NAME} STATIC ${SRCS} ${HEADERS})

#------------------------------------------------------------------------------
