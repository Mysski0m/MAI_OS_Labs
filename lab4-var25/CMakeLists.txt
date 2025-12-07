cmake_minimum_required(VERSION 3.20)
project(LabDynamicLibs LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_RPATH "$ORIGIN")

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(gcf_euclid SHARED src/gcf_euclid.cpp)
add_library(gcf_naive SHARED src/gcf_naive.cpp)
add_library(translation_binary SHARED src/translation_binary.cpp)
add_library(translation_ternary SHARED src/translation_ternary.cpp)

# Программа №1 — линковка (Евклид + двоичная)
add_executable(main_link apps/main_link.cpp)
target_link_libraries(main_link gcf_euclid translation_binary)

# Программа №1 — линковка (наивная + троичная)
add_executable(main_link_alt apps/main_link_alt.cpp)
target_link_libraries(main_link_alt gcf_naive translation_ternary)

# Программа №2 — runtime
add_executable(main_runtime
    apps/main_runtime.cpp
    src/platform/dynlib.cpp
)
target_link_libraries(main_runtime dl)