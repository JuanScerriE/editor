cmake_minimum_required(VERSION 3.10)

project(editor C)

set(CMAKE_C_STANDARD 17)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c17 -Wextra -Werror -Wall -pedantic -g")

add_executable(editor src/editor.c)
