#include "VeApp.hpp"

namespace ve {

void VeApp::run() {
  while (!veWindow.shouldClose()) {
    veWindow.pollEvents();
  }
}

} // namespace ve
