cmake_minimum_required(VERSION 3.0)
project(wakeup
        DESCRIPTION "Wake your computer from sleep by sending a Magic Packet"
        LANGUAGES CXX
        VERSION 0.0.1)

set(Boost_NO_BOOST_CMAKE On)
set(CMAKE_CXX_EXTENSIONS Off)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

include(./cmake/CPM.cmake)

set(BOOST_VERSION 1.67.0)
find_package(Boost ${BOOST_VERSION} QUIET)
if (NOT ${Boost_FOUND})
  CPMAddPackage(
          NAME boost-cmake
          GITHUB_REPOSITORY Orphis/boost-cmake
          VERSION ${BOOST_VERSION}
  )
endif ()

set(CXXOPTS_VERSION 2.2.0)
find_package(cxxopts ${CXXOPTS_VERSION} QUIET)
if (NOT ${cxxopts_FOUND})
  CPMAddPackage(
          NAME cxxopts
          GITHUB_REPOSITORY jarro2783/cxxopts
          VERSION ${CXXOPTS_VERSION}
          OPTIONS
          "CXXOPTS_BUILD_EXAMPLES Off"
          "CXXOPTS_BUILD_TESTS Off"
  )
endif ()

set(FMT_VERSION 7.0.1)
find_package(fmt ${FMT_VERSION} QUIET)
if (NOT ${fmt_FOUND})
  CPMAddPackage(
          NAME fmt
          GITHUB_REPOSITORY fmtlib/fmt
          GIT_TAG ${FMT_VERSION}
  )
endif ()

find_package(Threads REQUIRED)

add_executable(wakeup main.cpp)
target_compile_definitions(wakeup PRIVATE -DPROGRAM_VERSION="${CMAKE_PROJECT_VERSION}")
target_link_libraries(wakeup PRIVATE Boost::boost cxxopts fmt Threads::Threads)
