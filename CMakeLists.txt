cmake_minimum_required(VERSION 3.19)
project(FSM)

set(CMAKE_CXX_STANDARD 98)

add_executable(FSM main.cpp CHttpRequest.cpp CHttpRequest.hpp main.cpp)