cmake_minimum_required(VERSION 3.26)
project(parser)

add_compile_options( -std=c++20 -O3 -fopenmp )

include_directories(./include)

aux_source_directory(./src SRCS1)
add_executable(parser ${SRCS1})