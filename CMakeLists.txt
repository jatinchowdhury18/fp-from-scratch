cmake_minimum_required(VERSION 3.27)
project(fp_from_scratch)

set(CMAKE_CXX_STANDARD 20)

add_library(fp_from_scratch INTERFACE)
target_include_directories(fp_from_scratch INTERFACE include)

add_executable(basic_test main.cpp)
target_link_libraries(basic_test PRIVATE fp_from_scratch)
