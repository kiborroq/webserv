cmake_minimum_required(VERSION 3.19)
project(web)

set(CMAKE_CXX_STANDARD 98)

add_executable(web main.cpp Config.hpp Config.cpp Socket.cpp Socket.hpp utils.hpp)