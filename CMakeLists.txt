cmake_minimum_required(VERSION 3.19)
project(web)

set(CMAKE_CXX_STANDARD 98)

add_executable(web main.cpp Config.hpp Config.cpp ServerSocket.cpp ServerSocket.hpp parsing_utils.hpp parsing_utils.cpp PollFDs.cpp PollFDs.hpp WebServer.cpp WebServer.hpp ClientSocket.cpp ClientSocket.hpp Logger.cpp Logger.hpp Exceptions.cpp Exceptions.hpp)