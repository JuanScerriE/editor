#include "VeApp.hpp"
#include <vulkan/vulkan_core.h>
#include <stdexcept>

namespace ve {

VeApp::VeApp() {
  createPipelineLayout();
  createPipeline();
  createCommandBuffers();
}

VeApp::~VeApp() {
  vkDestroyPipelineLayout(veDevice.device(), pipelineLayout, nullptr);
}

void VeApp::run() {
  while (!veWindow.shouldClose()) {
    veWindow.pollEvents();
  }
}

void VeApp::createPipelineLayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(veDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout");
  }
}

void VeApp::createPipeline() {
  auto pipelineConfig = VePipeline::defaultPipelineConfigInfo(veSwapChain.width(), veSwapChain.height());
  pipelineConfig.renderPass = veSwapChain.getRenderPass();
  pipelineConfig.piplineLayout = pipelineLayout;
  vePipeline = std::make_unique<VePipeline>(veDevice,
      "res/shaders/simple.vert.spv",
      "res/shaders/simple.frag.spv",
      pipelineConfig);
}

void VeApp::drawFrame() {}
void VeApp::createCommandBuffers() {}

} // namespace ve
