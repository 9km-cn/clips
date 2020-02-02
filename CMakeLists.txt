cmake_minimum_required (VERSION 2.6)

set (MODULE_NAME clips)
project (${MODULE_NAME})

# def
if (NOT WIN32)
set (CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g3 -ggdb")
set (CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3")
set (CMAKE_CXX_FLAGS "-fPIC -m64 -Wall -std=c++11 ${CMAKE_CXX_FLAGS}")
endif ()
#add_definitions(-DEXPOERTS)

#------------------------------------------------------------------------------
# module

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(examples)

#------------------------------------------------------------------------------

if (TEST)
ADD_SUBDIRECTORY(test)
endif ()

#------------------------------------------------------------------------------
