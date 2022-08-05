#pragma once

#include <vulkan/vulkan_core.h>
#include "VePipeline.hpp"
#include "VeWindow.hpp"
#include "VeSwapChain.hpp"

// std
#include <memory>
#include <vector>

namespace ve {

class VeApp {
 public:
  static constexpr unsigned int WIDTH = 800;
  static constexpr unsigned int HEIGHT = 600;

  VeApp();
  ~VeApp();

  VeApp(const VeApp&) = delete;
  VeApp& operator=(const VeApp&) = delete;

  void run();

 private:
  void createPipelineLayout();
  void createPipeline();
  void createCommandBuffers();
  void drawFrame();

  VeWindow veWindow{WIDTH, HEIGHT, "Hallo Vulcano!"};
  VeDevice veDevice{veWindow};
  VeSwapChain veSwapChain{veDevice, veWindow.getExtent()};
  std::unique_ptr<VePipeline> vePipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;
};

}  // namespace ve
