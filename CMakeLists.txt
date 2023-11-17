cmake_minimum_required(VERSION 3.13.0)


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

project(leaf VERSION 1.0)

add_executable(leaf
    src/main.cpp
    src/const.cpp
    src/loader.cpp
    src/renderer.cpp
    src/physics.cpp
    src/game.cpp
    src/helpers.cpp
)

add_compile_options(-Wall -g)
target_include_directories(leaf PRIVATE include)

find_package(OpenGL REQUIRED)

add_subdirectory(libs/glfw)
target_link_libraries(leaf glfw)

add_subdirectory(libs/glad)
target_link_libraries(leaf glad)

add_subdirectory(libs/glm)
target_link_libraries(leaf glm)
