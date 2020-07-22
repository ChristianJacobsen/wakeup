#!/usr/bin/env sh

# Exit if CMake version is already cached
cache has_key "${CMAKE_CACHE_STRING}" && exit 0

# Download and extract desired version
wget -q "https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.tar.gz"
tar zxf "cmake-${CMAKE_VERSION}-Linux-x86_64.tar.gz"

# Store the newly downloaded version in the cache
cache store "${CMAKE_CACHE_STRING}" "./cmake-${CMAKE_VERSION}-Linux-x86_64/"
