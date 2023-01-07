cmake_minimum_required(VERSION 3.24)
project(CRS2 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-fopenmp")

include_directories(.)

add_executable(CRS2
        main.c src/functions/testFunctions.h src/functions/testFunctions.c src/functions/functionsChecks.c src/functions/functionsChecks.h)
