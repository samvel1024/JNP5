cmake_minimum_required(VERSION 3.10)
project(JNP5)

set(CMAKE_CXX_STANDARD 17)

#add_definitions(-DDEBUG)
#add_definitions(-DINPUTFILE="../std.in")
add_executable(generate dag_generator.c)
add_executable(test_transaction test_transaction.cpp citation_graph.h)
add_executable(test_create test_create.cpp citation_graph.h)
add_executable(test_official test_official.cpp citation_graph.h)
add_executable(test_dag_operations test_dag_operations.cpp citation_graph.h dag.h Publication.h)
add_executable(unit_tests unit_tests.cpp citation_graph.h)
add_executable(test_exception test_exception.cpp)
