cmake_minimum_required(VERSION 3.10)

# Set project name
project(Tutorial VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Tutorial main.cpp json.hpp)