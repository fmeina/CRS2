cmake_minimum_required(VERSION 3.24)
project(CRS2 C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-lm")
set(CMAME_C_FLAGS_RELEASE "-O3")
include_directories(.)
#include_directories(C:/apps/msys64/usr/include)
#link_directories(C:/apps/msys64/usr/lib)
FILE(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*.c)
add_executable(CRS2 ${SRC} src/common.h)

find_package(OpenMP REQUIRED)
find_package(MPI REQUIRED)
target_link_libraries(CRS2 PUBLIC MPI::MPI_C)
target_link_libraries(CRS2 PUBLIC OpenMP::OpenMP_C)




