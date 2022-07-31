#include "VeApp.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main() {
  std::cout << "Start of Normal section..." << std::endl;
  std::cout << "Start of Vulkan section..." << std::endl;

  ve::VeApp app{};

  try {
    app.run();
  } catch (const std::exception& except) {
    std::cerr << except.what() << std::endl;    
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
