# src CMakeLists.txt

add_executable(vktest
    Main.cpp
)

find_package(Vulkan REQUIRED)

target_include_directories(vktest PUBLIC include)
target_include_directories(vktest PRIVATE .)

target_link_libraries(vktest PRIVATE glfw)
target_link_libraries(vktest PRIVATE glm)
target_link_libraries(vktest PRIVATE Vulkan::Vulkan)
