#!/usr/bin/env sh

# Restore CMake from cache
cache restore "${CMAKE_CACHE_STRING}"

# Configure build
"./cmake-${CMAKE_VERSION}-Linux-x86_64/bin/cmake" -B build -DCMAKE_BUILD_TYPE=Release .

# Build
"./cmake-${CMAKE_VERSION}-Linux-x86_64/bin/cmake" --build build -j 3

# Store the built binary as an artifact
artifact push workflow build/wakeup
