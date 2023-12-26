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
    src/body.cpp
    src/geometry.cpp
    src/const.cpp
    src/loader.cpp
    src/renderer.cpp
    src/mmath.cpp
    src/physics.cpp
    src/game.cpp
    src/helpers.cpp
)

if(MSVC)
  target_compile_options(leaf PRIVATE /W4 /WX)
else()
  target_compile_options(leaf PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(leaf PRIVATE include)

find_package(OpenGL REQUIRED)

add_subdirectory(libs/glfw)
target_link_libraries(leaf glfw)

add_subdirectory(libs/glad)
target_link_libraries(leaf glad)

add_subdirectory(libs/glm)
target_link_libraries(leaf glm)
