cmake_minimum_required(VERSION 3.13)
project(JNP5)

set(CMAKE_CXX_STANDARD 17)

add_executable(JNP5 main.cpp CitationGraph.h)
add_executable(test_sandbox sandbox.cpp)