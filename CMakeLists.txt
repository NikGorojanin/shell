cmake_minimum_required(VERSION 3.12)
project(shenn)

set(CMAKE_CXX_STANDARD 14)

add_executable(shenn main.cpp shell.cpp shell.h utils.h)

set(boost_min_ver 1.50.0)
set(boost_libs system filesystem)
find_package(Boost ${boost_min_ver})

if(Boost_FOUND)
    find_package(Boost ${boost_min_ver} COMPONENTS ${boost_libs})
endif()
