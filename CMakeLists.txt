cmake_minimum_required(VERSION 3.13.0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

project(crunch VERSION 1.0)

add_executable(crunch
    src/main.cpp
    src/loader.cpp
    src/game.cpp
    src/renderer.cpp
)

add_compile_options(-Wall -g)
target_include_directories(crunch PRIVATE include)

find_package(OpenGL REQUIRED)

add_subdirectory(libs/glfw)
target_link_libraries(crunch glfw)

add_subdirectory(libs/glad)
target_link_libraries(crunch glad)

add_subdirectory(libs/glm)
target_link_libraries(crunch glm)
