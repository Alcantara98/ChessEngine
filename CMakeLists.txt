cmake_minimum_required(VERSION 3.10)
project(chess_engine)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Create compile_commands.json for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")

# Option to enable AddressSanitizer
option(ASAN "Enable AddressSanitizer" OFF)

# Option to create an optimized release build
option(RELEASE "Enable Release Optimization" OFF)

option(BUILD_CLI_APP "Build CLI app (main_cli.cpp)" ON)
option(BUILD_UCI_APP "Build UCI app (main_uci.cpp)" ON)

# Add source files
file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp")
list(REMOVE_ITEM ENGINE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main_cli.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main_uci.cpp"
)


# Static linking for MinGW/GCC
if(MINGW)
    message(STATUS "Using MinGW - enabling static linking")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")
    set(EXTRA_LIBS -lwinmm -lws2_32 -static-libgcc -static-libstdc++)
endif()

# If ASan is enabled, add the necessary flags
if(ASAN)
    message(STATUS "Building with AddressSanitizer (ASan)")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g -O1")
endif()

# Set build type to Release and add optimization flags
if(RELEASE)
    message(STATUS "Building with high optimization")
    set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -flto")

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_AR "gcc-ar")
        set(CMAKE_RANLIB "gcc-ranlib")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_AR "llvm-ar")
        set(CMAKE_RANLIB "llvm-ranlib")
    endif()
endif()

# Shared engine library
add_library(chess_engine_core STATIC ${ENGINE_SOURCES})

if(EXTRA_LIBS)
    target_link_libraries(chess_engine ${EXTRA_LIBS})
endif()

# App 1: existing CLI main
if(BUILD_CLI_APP)
    add_executable(chess_engine_cli src/main_cli.cpp)
    target_link_libraries(chess_engine_cli PRIVATE chess_engine_core)
endif()

# App 2: UCI main
if(BUILD_UCI_APP)
    add_executable(chess_engine_uci src/main_uci.cpp)
    target_link_libraries(chess_engine_uci PRIVATE chess_engine_core)
endif()
