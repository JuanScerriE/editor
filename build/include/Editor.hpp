#pragma once

#define Editor_VERSION_MAJOR 0
#define Editor_VERSION_MINOR 1

#include <GLFW/glfw3.h>
#include <iostream>

void test() {
    std::cout << "Hello I am working! " << Editor_VERSION_MAJOR << "." << Editor_VERSION_MINOR << "\n";
}
