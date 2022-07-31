#pragma once

#include "VePipeline.hpp"
#include "VeWindow.hpp"

namespace ve {

class VeApp {
 public:
  static constexpr unsigned int WIDTH = 800;
  static constexpr unsigned int HEIGHT = 600;

  void run();

 private:
  VeWindow veWindow{WIDTH, HEIGHT, "Hallo Vulcano!"};
  VeDevice veDevice{veWindow};
  VePipeline vePipeline{
      veDevice, "res/shaders/simple.vert.spv",
      "res/shaders/simple.frag.spv",
      VePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
};

}  // namespace ve
