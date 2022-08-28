cmake_minimum_required(VERSION 3.10)

# Project name
# project(mcpp_client VERSION 0.01)
project(mcpp_client)

# add main.cpp
add_executable(mcpp_client main.cpp)

add_library(glad glad/src/glad.c)

target_link_libraries(mcpp_client glfw)
target_link_libraries(mcpp_client GL)
target_link_libraries(mcpp_client glad)
target_include_directories(mcpp_client PRIVATE ${include})
