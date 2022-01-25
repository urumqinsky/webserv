cmake_minimum_required(VERSION 3.20)
project(webserv)

set(CMAKE_CXX_STANDARD 98)

include_directories(.)

add_executable(webserv main.cpp makeQueue.cpp Request.cpp Request.hpp ServerSocket.cpp ServerSocket.hpp
        ServerConfig.cpp ServerConfig.hpp libraryHeaders.hpp functions.hpp testConfigParser.cpp cgi.cpp)