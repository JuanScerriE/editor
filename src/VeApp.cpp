#include "VeApp.hpp"

#include "VePipeline.hpp"

// vulkan
#include <vulkan/vulkan_core.h>

// lib
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// c std
#include <stdint.h>

// std
#include <array>
#include <memory>
#include <stdexcept>

namespace ve {

struct SimplePushConstantData {
  glm::mat2 transform{1.0f};
  glm::vec2 offset;
  alignas(16) glm::vec3 color;
};

struct Triangle {
  VeModel::Vertex v0;
  VeModel::Vertex v1;
  VeModel::Vertex v2;
};

struct Triangle3 {
  Triangle t0;
  Triangle t1;
  Triangle t2;
};

std::vector<VeModel::Vertex> convertTrianglesToVertices(
    const std::vector<Triangle>& triangles) {
  std::vector<VeModel::Vertex> vertices{};

  for (const auto& triangle : triangles) {
    vertices.push_back(std::move(triangle.v0));
    vertices.push_back(std::move(triangle.v1));
    vertices.push_back(std::move(triangle.v2));
  }

  return vertices;
}

Triangle3 calcSerpinskiVertices(Triangle triangle) {
  VeModel::Vertex m0{};
  VeModel::Vertex m1{};
  VeModel::Vertex m2{};

  m0.position =
      (triangle.v0.position + triangle.v1.position) / 2.0f;
  m1.position =
      (triangle.v1.position + triangle.v2.position) / 2.0f;
  m2.position =
      (triangle.v2.position + triangle.v0.position) / 2.0f;

  Triangle3 triangles{
      {triangle.v0, m0, m2},
      {m0, triangle.v1, m1},
      {m2, m1, triangle.v2},
  };

  return triangles;
}

std::vector<VeModel::Vertex> innerSerpinskiTriangle(
    int n, const std::vector<Triangle>& triangles) {
  if (n == 1) {
    return convertTrianglesToVertices(triangles);
  } else {
    std::vector<Triangle> newTriangles{};
    for (const auto& triangle : triangles) {
      Triangle3 calculatedTriangles =
          calcSerpinskiVertices(triangle);
      newTriangles.push_back(calculatedTriangles.t0);
      newTriangles.push_back(calculatedTriangles.t1);
      newTriangles.push_back(calculatedTriangles.t2);
    }
    return innerSerpinskiTriangle(n - 1, newTriangles);
  }
}

std::vector<VeModel::Vertex> serpinskiTriangle(
    int n, const Triangle& triangle) {
  return innerSerpinskiTriangle(
      n, std::vector<Triangle>{triangle});
}

VeApp::VeApp() {
  std::cout
      << "Maximum Push constant size: "
      << veDevice.properties.limits.maxPushConstantsSize
      << std::endl;

  loadModels();
  createPipelineLayout();
  recreateSwapChain();
  createCommandBuffers();
}

VeApp::~VeApp() {
  vkDestroyPipelineLayout(veDevice.device(), pipelineLayout,
                          nullptr);
}

void VeApp::run() {
  while (!veWindow.shouldClose()) {
    veWindow.pollEvents();
    drawFrame();
  }

  vkDeviceWaitIdle(veDevice.device());
}

void VeApp::loadModels() {
  // Triangle triangle{
  //     {{0.0f, -0.5f}},
  //     {{0.5f, 0.5f}},
  //     {{-0.5f, 0.5f}},
  // };

  // std::vector<VeModel::Vertex> vertices =
  // serpinskiTriangle(5, triangle);
  std::vector<VeModel::Vertex> vertices{
      {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
      {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
  };

  veModel = std::make_unique<VeModel>(veDevice, vertices);
}

void VeApp::createPipelineLayout() {
  assert(
      sizeof(SimplePushConstantData) <=
          veDevice.properties.limits.maxPushConstantsSize &&
      "exceeded maximum push constant size");

  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT |
      VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(SimplePushConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges =
      &pushConstantRange;

  if (vkCreatePipelineLayout(
          veDevice.device(), &pipelineLayoutInfo, nullptr,
          &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error(
        "failed to create pipeline layout");
  }
}

void VeApp::createPipeline() {
  assert(veSwapChain != nullptr &&
         "cannot create pipeline before swap chain");
  assert(pipelineLayout != nullptr &&
         "cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  VePipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = veSwapChain->getRenderPass();
  pipelineConfig.piplineLayout = pipelineLayout;
  vePipeline = std::make_unique<VePipeline>(
      veDevice, "res/shaders/simple.vert.spv",
      "res/shaders/simple.frag.spv", pipelineConfig);
}

void VeApp::createCommandBuffers() {
  commandBuffers.resize(veSwapChain->imageCount());
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType =
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = veDevice.getCommandPool();
  allocInfo.commandBufferCount =
      static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(
          veDevice.device(), &allocInfo,
          commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error(
        "failed to allocate command buffers");
  }
}

void VeApp::freeCommandBuffers() {
  vkFreeCommandBuffers(
      veDevice.device(), veDevice.getCommandPool(),
      static_cast<uint32_t>(commandBuffers.size()),
      commandBuffers.data());
  commandBuffers.clear();
}

void VeApp::recreateSwapChain() {
  auto extent = veWindow.getExtent();
  // Wait to make sure it is not minimised.
  if (extent.width == 0 || extent.height == 0) {
    extent = veWindow.getExtent();
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(veDevice.device());
  if (veSwapChain == nullptr) {
    veSwapChain =
        std::make_unique<VeSwapChain>(veDevice, extent);
  } else {
    veSwapChain = std::make_unique<VeSwapChain>(
        veDevice, extent, std::move(veSwapChain));
    if (veSwapChain->imageCount() !=
        commandBuffers.size()) {
      freeCommandBuffers();
      createCommandBuffers();
    }
  }

  // TODO: if render passes are compatible no need to
  // recreate pipeline.

  createPipeline();
}

void VeApp::recordCommandBuffer(uint32_t imageIndex) {
  static int frame = 0;
  frame = (frame + 1) % 100;

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType =
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffers[imageIndex],
                           &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error(
        "failed to start recording command buffer");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType =
      VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = veSwapChain->getRenderPass();
  renderPassInfo.framebuffer =
      veSwapChain->getFrameBuffer(imageIndex);
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent =
      veSwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount =
      static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffers[imageIndex],
                       &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(
      veSwapChain->getSwapChainExtent().width);
  viewport.height = static_cast<float>(
      veSwapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffers[imageIndex], 0, 1,
                   &viewport);

  VkRect2D scissor{{0, 0},
                   veSwapChain->getSwapChainExtent()};
  vkCmdSetScissor(commandBuffers[imageIndex], 0, 1,
                  &scissor);

  vePipeline->bind(commandBuffers[imageIndex]);
  veModel->bind(commandBuffers[imageIndex]);

  for (int i = 0; i < 4; i++) {
    SimplePushConstantData pushConstantData{};
    pushConstantData.offset = {-0.4f + frame * 0.02,
                               -0.4f + i * 0.25f};
    pushConstantData.color = {0.0f, 0.0f, 0.1f * i};

    vkCmdPushConstants(commandBuffers[imageIndex],
                       pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData),
                       &pushConstantData);

    veModel->draw(commandBuffers[imageIndex]);
  }

  vkCmdEndRenderPass(commandBuffers[imageIndex]);

  if (vkEndCommandBuffer(commandBuffers[imageIndex]) !=
      VK_SUCCESS) {
    throw std::runtime_error(
        "failed to record command buffer");
  }
}

void VeApp::drawFrame() {
  uint32_t imageIndex;
  auto result = veSwapChain->acquireNextImage(&imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error(
        "failed to acquire swap chain image");
  }

  recordCommandBuffer(imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR ||
      result == VK_SUBOPTIMAL_KHR ||
      veWindow.wasWindowResized()) {
    veWindow.resetWindowResizedFlag();
    recreateSwapChain();
    return;
  }

  if (veSwapChain->submitCommandBuffers(
          &commandBuffers[imageIndex], &imageIndex) !=
      VK_SUCCESS) {
    throw std::runtime_error(
        "failed to submit command buffer");
  }
}

}  // namespace ve
