cmake_minimum_required(VERSION 3.16)
project(Spp)

set(CMAKE_CXX_STANDARD 17)

add_executable(Spp main.cpp Symbolic/Number.h Symbolic/Compute.h)
