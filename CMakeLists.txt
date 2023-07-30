cmake_minimum_required(VERSION 3.26)
project(parser)

FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
add_compile_options( -std=c++20 -O3 -fopenmp )

include_directories(./include)

aux_source_directory(./src SRCS1)
add_executable(parser ${SRCS1})