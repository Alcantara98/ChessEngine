.PHONY: build build-release build-debug

CC := clang
CXX := clang++

# BUILD COMMANDS
build:
	cmake -B build -DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX)
	cmake --build build --parallel

build-release:
	cmake -B build -DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX) -DCMAKE_BUILD_TYPE=Release -DRELEASE=ON -DASAN=OFF
	cmake --build build --parallel

build-debug:
	cmake -B build -DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX) -DCMAKE_BUILD_TYPE=Debug -DASAN=ON
	cmake --build build --parallel

# LINTING COMMANDS
format:
	clang-format --version
	clang-format -i src/*.cpp src/*.h

tidy:
	cmake -B build -DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX)
	clang-tidy --version
	clang-tidy -p build src/*.cpp src/*.h
