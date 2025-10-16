cmake_minimum_required(VERSION 3.10)
project(lab2_var15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=maybe-uninitialized")

add_executable(main main.cpp src/deck.cpp src/monte_carlo_card.cpp)

target_include_directories(main PRIVATE include)

find_package(Threads REQUIRED)
target_link_libraries(main PRIVATE Threads::Threads)