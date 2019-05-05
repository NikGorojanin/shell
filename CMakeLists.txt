cmake_minimum_required(VERSION 3.12)
project(shell)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)

add_executable(shell main.cpp shell.cpp shell.h utils.h Conditional.cpp Conditional.h conditional.cpp conditional.h)

find_package(Boost COMPONENTS system filesystem REQUIRED)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    target_link_libraries(shell
            ${Boost_FILESYSTEM_LIBRARY}
            ${Boost_SYSTEM_LIBRARY}
            )
endif()