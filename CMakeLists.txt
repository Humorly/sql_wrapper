
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

project ("sql_wrapper")

include_directories(./mysql8.2/include/jdbc)
link_directories(./mysql8.2/lib64/vs14)

add_executable (sql_wrapper "sql_warpper_test.cpp")