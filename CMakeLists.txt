cmake_minimum_required(VERSION 3.24)
project(WebServ_master)

set(CMAKE_CXX_STANDARD 14)

include_directories(parsing)

add_executable(WebServ_master
        errorsHandling/errorsHandling.cpp
        parsing/locationparse.cpp
        parsing/parsing.cpp
        parsing/parsing.hpp
        parsing/struct_methods_implementation.cpp
        parsing/web_server.cpp
        testcpp/copy_test.cpp
        testcpp/test.cpp
        copy_test.cpp
        server_test.cpp
        test.cpp locationHandling.cpp sendingErrorPages.cpp handlingPostRequest.cpp handlingPostRequest.cpp)
