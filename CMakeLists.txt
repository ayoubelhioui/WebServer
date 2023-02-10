cmake_minimum_required(VERSION 3.24)
project(WebServer)

set(CMAKE_CXX_STANDARD 14)

include_directories(parsing)

add_executable(WebServer
        errorsHandling/errorsHandling.cpp
        parsing/locationparse.cpp
        parsing/parsing.cpp
        parsing/parsing.hpp
        parsing/struct_methods_implementation.cpp
        parsing/web_server.cpp
        public/kk.cpp
        testcpp/copy_test.cpp
        copy_test.cpp
        getMethod.cpp
        handlingPostRequest.cpp
        locationHandling.cpp
        sendingErrorPages.cpp)
