message(STATUS "Fetching Glad...")

# add_subdirectory(src)

add_library(glad STATIC src/glad.c)

target_include_directories(glad PUBLIC include)