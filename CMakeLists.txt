cmake_minimum_required(VERSION 2.8.9)

project( ifcconvert )
 
# Bringing all the headers into the project
include_directories(include)
 
#However, the file(GLOB...) allows for wildcard additions:
file( GLOB SOURCES "*.cpp" )
 
# Attaching the shared libraries:
set( PROJECT_LINK_LIBS ifc2x3.so Step.so)
# Linking the directories:
link_directories( src/ifc2x3/build src/Step/build )
 
add_executable( ifcconvert ${SOURCES} ) # BRepExample.cpp BrepReaderVisitor.cpp)

target_link_libraries( ifcconvert ${PROJECT_LINK_LIBS} )
