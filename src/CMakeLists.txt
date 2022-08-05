# src CMakeLists.txt

add_executable(editor
  Editor.cpp
  VeApp.cpp
  VeDevice.cpp
  VeWindow.cpp
  VePipeline.cpp
  VeSwapChain.cpp
)

find_package(Vulkan REQUIRED FATA_ERROR)

target_include_directories(editor PUBLIC include)
target_include_directories(editor PRIVATE .)

target_link_libraries(editor PRIVATE glfw)
target_link_libraries(editor PRIVATE glm)
target_link_libraries(editor PRIVATE Vulkan::Vulkan)

if(UNIX)
  if (NOT APPLE)
    target_compile_options(editor PRIVATE "-ldl;-lpthread;-lX11;-lXxf86vm;-lXrandr;-lXi")
  endif()
endif()
