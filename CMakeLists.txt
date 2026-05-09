cmake_minimum_required(VERSION 3.10)
project(BaiCaoCaiSim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

# Gather all source files
file(GLOB SOURCES "src/*.cpp")
list(APPEND SOURCES "main.cpp")

add_executable(BaiCaoCaiSim ${SOURCES})
