cmake_minimum_required( VERSION 3.16 )

set(CMAKE_CXX_STANDARD 11)

project(
    TRECS
    VERSION 0.0.1
)

option(BUILD_TRECS_TESTS "Build tests with the ECS framework" ON)

include( ${CMAKE_SOURCE_DIR}/cmake/functions.cmake )

add_subdirectory( trecs )
add_subdirectory( tests )
