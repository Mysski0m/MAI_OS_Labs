cmake_minimum_required(VERSION 3.10)
project(lab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=maybe-uninitialized")

add_library(child_lib src/child.cpp)
add_library(parent_lib src/parent.cpp)
add_library(os_lib src/os.cpp)

target_link_libraries(parent_lib PUBLIC os_lib)
target_link_libraries(child_lib PUBLIC os_lib)

target_include_directories(child_lib PUBLIC include)
target_include_directories(parent_lib PUBLIC include)
target_include_directories(os_lib PUBLIC include)

add_executable(child bin/child.cpp)
target_link_libraries(child PRIVATE child_lib rt pthread)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE parent_lib rt pthread)