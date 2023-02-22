cmake_minimum_required(VERSION 3.24)
project(NewWebServer)

set(CMAKE_CXX_STANDARD 17)

add_executable(NewWebServer main.cpp Implementations/HttpServer.cpp)
