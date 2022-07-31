#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

namespace ve {

class VeWindow {
  public:
    VeWindow(unsigned int w, unsigned int h, const std::string& name);

    VeWindow(const VeWindow&) = delete;
    VeWindow& operator=(const VeWindow&) = delete;

    ~VeWindow();

    bool shouldClose() const {
      return glfwWindowShouldClose(window);
    }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    void pollEvents() const {
      glfwPollEvents();
    }

  private:
    void initWindow();

    const unsigned int width;
    const unsigned int height;

    std::string windowName;
    GLFWwindow *window = nullptr;
};

} // namespace ve
