#include "VeWindow.hpp"
#include <vulkan/vulkan_core.h>

#include <stdexcept>

namespace ve {

VeWindow::VeWindow(unsigned int w, unsigned int h, const std::string& name)
: width(w), height(h), windowName(name) {
  initWindow();
}

VeWindow::~VeWindow() {
  if (window != nullptr) {
    glfwDestroyWindow(window);
  }

  glfwTerminate();
}

void VeWindow::initWindow() {
  if (!glfwInit()) {
    throw std::runtime_error("failed to initialise GLFW");
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  
  window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

  if (!window) {
    throw std::runtime_error("failed to create GLFW window");
  }
}

void VeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
  if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface");
  }
}

} // namespace ve
