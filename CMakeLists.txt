cmake_minimum_required(VERSION 3.10)

# Project name
# project(mcpp_client VERSION 0.01)
project(mcpp_client)
set(mcpp_client_VERSION_MAJOR "0")
set(mcpp_client_VERSION_MINOR "01")

configure_file(version.hpp.in version.hpp)

# add main.cpp
add_executable(mcpp_client main.cpp)

target_include_directories(mcpp_client PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )
target_link_libraries(mcpp_client glfw)
target_link_libraries(mcpp_client GL)
