cmake_minimum_required(VERSION 3.14)

project(
  editor
  VERSION 0.1
  LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(deps)
add_subdirectory(src)
