cmake_minimum_required( VERSION 3.16 )

set( CMAKE_CXX_STANDARD 11 )

project(
    TRECS
    VERSION 0.0.1
)

enable_testing()

option( BUILD_TRECS_TESTS "Build tests with the ECS framework" ON )

# Some of the template instantiations make very big .obj files, but this only
# seems to be a problem on Windows with mingw.
if( WIN32 )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj" )
endif()

include( ${CMAKE_SOURCE_DIR}/cmake/functions.cmake )

set( LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}" )
set( EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}" )

add_subdirectory( trecs )

# Disable this in a higher level by writing:
#    set( BUILD_TRECS_TESTS 0 )
if ( BUILD_TRECS_TESTS )
    add_subdirectory( tests )
endif()
