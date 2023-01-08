cmake_minimum_required(VERSION 3.24)
project(CRS2 C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-fopenmp -lm")
set(CMAME_C_FLAGS_RELEASE "-O3")

include_directories(.)

FILE(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*.c)
add_executable(CRS2 ${SRC})
