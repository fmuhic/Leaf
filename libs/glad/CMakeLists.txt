cmake_minimum_required(VERSION 3.13)

add_library(glad glad.c)

target_include_directories(glad PUBLIC include)
