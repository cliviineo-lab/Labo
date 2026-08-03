cmake_minimum_required(VERSION 3.20)
project(SwarmOS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/SwarmOS/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()

add_executable(test_kernel SwarmOS/tests/test_kernel.cpp)
add_test(NAME test_kernel COMMAND test_kernel)

add_executable(test_cbf SwarmOS/tests/test_cbf.cpp)
add_test(NAME test_cbf COMMAND test_cbf)
