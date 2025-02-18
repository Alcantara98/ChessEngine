cmake_minimum_required(VERSION 3.10)
project(chess_engine)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")

# Option to enable AddressSanitizer
option(ASAN "Enable AddressSanitizer" OFF)

# Option to enable high optimization
option(OPTIMIZE "Enable high optimization" OFF)

# Include directories
include_directories(include)

# Add source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# If ASan is enabled, add the necessary flags
if(ASAN)
    message(STATUS "Building with AddressSanitizer (ASan)")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g -O1")
endif()

# If high optimization is enabled, set build type to Release and add optimization flags
if(OPTIMIZE)
    message(STATUS "Building with high optimization")
    set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -flto")
endif()

# Create the executable
add_executable(chess_engine ${SOURCES})
