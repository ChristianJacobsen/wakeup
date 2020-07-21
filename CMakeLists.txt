cmake_minimum_required(VERSION 3.0)
project(wakeup
        DESCRIPTION "Wake your computer from sleep by sending a Magic Packet"
        LANGUAGES CXX
        VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

add_subdirectory(./external/cxxopts)
add_subdirectory(./external/fmt)
find_package(Threads REQUIRED)

add_executable(wakeup main.cpp)
target_compile_definitions(wakeup PRIVATE -DPROGRAM_VERSION="${CMAKE_PROJECT_VERSION}")
target_include_directories(wakeup PRIVATE ./external/algorithm/include)
target_include_directories(wakeup PRIVATE ./external/asio/include)
target_link_libraries(wakeup PRIVATE cxxopts fmt Threads::Threads)
