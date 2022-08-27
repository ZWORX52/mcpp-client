cmake_minimum_required(VERSION 3.10)

# Project name
project(mcpp_client VERSION 0.01)

configure_file(version.hpp.in version.hpp)

# add main.cpp
add_executable(mcpp_client main.cpp)

target_include_directories(mcpp_client PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )
