cmake_minimum_required(VERSION 3.13)
project(JNP5)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DDEBUG)

add_executable(JNP5 main.cpp citation_graph.h)
add_executable(test_transaction test_transaction.cpp citation_graph.h)
add_executable(test_create test_create.cpp citation_graph.h)
add_executable(test_official test_official.cpp citation_graph.h)