#pragma once

#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

namespace ve {

class VeWindow {
 public:
  VeWindow(unsigned int w, unsigned int h,
           const std::string& name);

  VeWindow(const VeWindow&) = delete;
  VeWindow& operator=(const VeWindow&) = delete;

  ~VeWindow();

  bool shouldClose() const {
    return glfwWindowShouldClose(window);
  }

  VkExtent2D getExtent() {
    return {static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)};
  }

  void createWindowSurface(VkInstance instance,
                           VkSurfaceKHR* surface);

  void pollEvents() const {
    glfwPollEvents();
  }

 private:
  void initWindow();

  const int width;
  const int height;

  std::string windowName;
  GLFWwindow* window = nullptr;
};

}  // namespace ve
