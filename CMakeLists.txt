cmake_minimum_required(VERSION 3.24)
project(NewWebServer)

set(CMAKE_CXX_STANDARD 17)

add_executable(NewWebServer main.cpp Implementations/HttpServer.cpp Implementations/HttpServer.cpp Implementations/POSTMethod.cpp errorsHandling/errorsHandling.cpp Implementations/GETMethod.cpp Implementations/ServerConfiguration.cpp Implementations/RequestParser.cpp Implementations/Client.cpp test.c Implementations/LocationBlockParse.cpp Implementations/LocationBlockParse.cpp Implementations/ChunkedPostRequest.cpp Implementations/DeleteMethod.cpp)
