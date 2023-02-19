cmake_minimum_required(VERSION 3.24)
project(WebServer)

set(CMAKE_CXX_STANDARD 14)

include_directories(parsing)

add_executable(WebServer
        errorsHandling/errorsHandling.cpp
        parsing/locationBlockParse.cpp
        Interfaces/ConfigFileParser.hpp
        parsing/ConfigFileParser.cpp
        parsing/web_server.cpp
        public/kk.cpp
        testcpp/copy_test.cpp
        main.cpp
        getMethod.cpp
        handlingPostRequest.cpp
        locationHandling.cpp
        sendingErrorPages.cpp includes/postRequest.hpp Interfaces/locationBlockParse.hpp parsing/getMethod.cpp parsing/parsingUtils.cpp)
