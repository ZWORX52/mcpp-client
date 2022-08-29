cmake_minimum_required(VERSION 3.10)

# Project name
# project(mcpp_client VERSION 0.01)
project(mcpp_client)

# add main.cpp
set(INCLUDE_DIR include)
set(SOURCES main.cpp ${INCLUDE_DIR}/glad/glad.h)
set(CMAKE_BUILD_TYPE Debug)
add_executable(mcpp_client ${SOURCES})

add_library(glad ${INCLUDE_DIR}/glad/glad.c)

target_link_libraries(mcpp_client glfw)
target_link_libraries(mcpp_client GL)
target_link_libraries(mcpp_client glad)
target_include_directories(mcpp_client PRIVATE ${INCLUDE_DIR})
target_include_directories(glad PRIVATE ${INCLUDE_DIR})
