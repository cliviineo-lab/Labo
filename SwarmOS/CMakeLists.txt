cmake_minimum_required(VERSION 3.14)
project(SwarmOS CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Lien avec libatomic pour les types std::atomic volumineux
link_libraries(atomic)

add_executable(test_kernel tests/test_kernel.cpp)
add_executable(test_cbf tests/test_cbf.cpp)
add_executable(test_bridge tests/test_bridge.cpp)

enable_testing()
add_test(NAME test_kernel COMMAND test_kernel)
add_test(NAME test_cbf COMMAND test_cbf)
add_test(NAME test_bridge COMMAND test_bridge)
