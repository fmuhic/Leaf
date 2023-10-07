cmake_minimum_required(VERSION 3.13.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

project(crunch VERSION 1.0)

add_executable(crunch
    src/main.cpp
    src/glad.c
)
target_include_directories(crunch PRIVATE include)

find_package(OpenGL REQUIRED)

add_subdirectory(libs/glfw)
target_link_libraries(crunch glfw)
