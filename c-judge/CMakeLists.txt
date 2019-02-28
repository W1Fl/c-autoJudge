cmake_minimum_required(VERSION 3.13)
project(c_judge)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(c_judge
        dir.h
        main.cpp
        process.h)
