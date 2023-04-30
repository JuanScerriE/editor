message(STATUS "Fetching GLFW...")

if(UNIX AND NOT APPLE)
    set(GLFW_USE_WAYLAND 1)
endif()

FetchContent_MakeAvailable(glfw)
