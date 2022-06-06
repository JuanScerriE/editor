cmake_minimum_required(VERSION 3.10)

macro(print_all_variables)
  message(STATUS "print_all_variables------------------------------------------{")
  get_cmake_property(_variableNames VARIABLES)
  foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
  message(STATUS "print_all_variables------------------------------------------}")
endmacro()

cmake_policy(SET CMP0079 NEW)

project(
  Editor
  VERSION 0.1
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Werror -Wall -pedantic")

print_all_variables()

find_library(GLFW_LIB glfw libglfw)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(tests)

target_link_libraries(Editor
  PUBLIC
    "${GLFW_LIB}"
)

target_include_directories(Editor
  PUBLIC
    "${PROJECT_BINARY_DIR}/include"
)
