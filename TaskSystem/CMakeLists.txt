# Root cmake file to build the library

cmake_minimum_required(VERSION 2.8.7)
project(TaskSystem C)

set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
#message(STATUS ${PROJECT_INCLUDE_DIR})

# Task system library
add_subdirectory(src)

# Test
add_subdirectory(test)