#pragma once

#include <vulkan/vulkan_core.h>
#include "VePipeline.hpp"
#include "VeWindow.hpp"
#include "VeSwapChain.hpp"
#include "VeModel.hpp"

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
  void loadModels();
  void createPipelineLayout();
  void createPipeline();
  void createCommandBuffers();
  void freeCommandBuffers();
  void drawFrame();
  void recreateSwapChain();
  void recordCommandBuffer(uint32_t imageIndex);

  VeWindow veWindow{WIDTH, HEIGHT, "Hallo Vulcano!"};
  VeDevice veDevice{veWindow};
  std::unique_ptr<VeSwapChain> veSwapChain;
  std::unique_ptr<VePipeline> vePipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;
  std::unique_ptr<VeModel> veModel;
};

}  // namespace ve
