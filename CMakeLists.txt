cmake_minimum_required(VERSION 3.14)

# macro(print_all_variables)
#   message(STATUS "print_all_variables------------------------------------------{")
#   get_cmake_property(_variableNames VARIABLES)
#   foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
#   endforeach()
#   message(STATUS "print_all_variables------------------------------------------}")
# endmacro()

project(
  editor
  VERSION 0.1
  LANGUAGES C CXX
)

# Export compile commands for clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

#set(OpenGL_GL_PREFERENCE "GLVND")

#find_package(OpenGL REQUIRED)
#find_library(GLFW_LIB glfw libglfw REQUIRED)

add_subdirectory(deps)
add_subdirectory(src)
