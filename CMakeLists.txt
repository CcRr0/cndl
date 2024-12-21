cmake_minimum_required(VERSION 3.30)
project(cndl)

set(CMAKE_CXX_STANDARD 26)

include_directories(include)

add_executable(cndl main.cpp
        src/bundle.cpp
        src/utility.cpp)
