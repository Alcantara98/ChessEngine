cmake_minimum_required(VERSION 3.10)
project(chess_engine)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Add source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create the executable
add_executable(chess_engine ${SOURCES})
