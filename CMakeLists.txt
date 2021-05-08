cmake_minimum_required(VERSION 3.16)
project(cat VERSION 0.1)
set(CMAKE_C_STANDARD 11)
add_executable(cat main.c die.c cat.c)
configure_file(config.h.in config.h)
target_include_directories(cat PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )
