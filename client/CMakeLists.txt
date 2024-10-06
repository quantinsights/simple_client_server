cmake_minimum_required(VERSION 3.30.3)
project(client)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
    src/main.cpp
)

add_executable(client ${SOURCES})	