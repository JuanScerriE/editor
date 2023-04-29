# src CMakeLists.txt

add_executable(editor
  Editor.cpp
  VeApp.cpp
  VeModel.cpp
  VeDevice.cpp
  VeWindow.cpp
  VePipeline.cpp
  VeSwapChain.cpp
)

find_package(Vulkan REQUIRED)

target_include_directories(editor PUBLIC include)
target_include_directories(editor PRIVATE .)

target_link_libraries(editor PRIVATE glfw)
target_link_libraries(editor PRIVATE glm)
target_link_libraries(editor PRIVATE Vulkan::Vulkan)
