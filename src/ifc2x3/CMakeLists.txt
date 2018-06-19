cmake_minimum_required(VERSION 2.8.9)
project( ifc2x3 )
 
# A library, not executable
set( CMAKE_BUILD_TYPE Release )
 
# Bringing the headers into the project 
include_directories( ../../include )
 
# Choosing the source files:
file(GLOB SOURCES "*.cpp")
 
# Generating a shared library from the sources
add_library( ifc2x3 SHARED ${SOURCES} )
 