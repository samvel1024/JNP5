cmake_minimum_required(VERSION 3.13)
project(JNP5)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DDEBUG)

add_executable(generate dag_generator.c)


add_executable(test_transaction test_transaction.cpp citation_graph.h)
add_executable(test_create test_create.cpp citation_graph.h)
add_executable(test_official test_official.cpp citation_graph.h)
add_executable(test_dag_operations test_dag_operations.cpp citation_graph.h dag.h publication.h)
