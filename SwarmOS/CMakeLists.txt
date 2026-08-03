cmake_minimum_required(VERSION 3.20)
project(SwarmOS VERSION 1.0.0 LANGUAGES CXX)

# Standard C++20 strict
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Flags d'optimisation et d'avertissement stricts pour l'embarquC) Temps RC)el
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -O3
    -ffast-math
    -fno-exceptions
    -fno-rtti
)

# Inclusions d'en-tC*tes
include_directories(include)

# Executable principal SwarmOS
add_executable(swarmos_node
    src/main.cpp
)

# Configuration CTest pour les futurs tests unitaires
enable_testing()

# Affichage du rC)sumC) de configuration
message(STATUS "Build SwarmOS mode: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
