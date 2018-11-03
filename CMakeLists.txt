cmake_minimum_required(VERSION 3.10)
project(sequence)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -fsanitize=address -D_GLIBCXX_DEBUG -Wextra")

add_executable(sequence seq_tests.cpp)
