// vulkan
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

// c standard
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <cstring>

// cpp standard
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

volatile sig_atomic_t gSignalStatus;

VkResult createDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(
            instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator,
                    pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(
            instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void ctrlCHandler(int sig) {
    gSignalStatus = sig;
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() &&
               presentFamily.has_value();
    }
};

QueueFamilyIndices findQueueFamilies(
    VkPhysicalDevice device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(
        queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        device, &queueFamilyCount, queueFamilies.data());

    int i = 0;

    for (auto queueFamily : queueFamilies) {
        if (queueFamily.queueFlags &
            VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            device, i, surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

class Application {
   public:
    void run() {
        initWindow();
        initCtrlCHandler();
        initVulkan();
        mainLoop();
        cleanup();
    }

   private:
    void initCtrlCHandler() {
        signal(SIGINT, ctrlCHandler);
    }

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "vktest",
                                  nullptr, nullptr);

        if (!window) {
            throw std::runtime_error(
                "glfw could not create window");
        }
    }

    void initVulkan() {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
    }

    void createSurface() {
        if (glfwCreateWindowSurface(instance, window,
                                    nullptr, &surface) !=
            VK_SUCCESS) {
            throw std::runtime_error(
                "failed to create window surface");
        }
    }

    void createLogicalDevice() {
        QueueFamilyIndices indices =
            findQueueFamilies(physicalDevice, surface);

        std::vector<VkDeviceQueueCreateInfo>
            queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value()};

        float queuePriority = 1.0f;

        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType =
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities =
                &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType =
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos =
            queueCreateInfos.data();
        createInfo.queueCreateInfoCount =
            static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = 0;

        if (enableValidationLayers) {
            createInfo.enabledLayerCount =
                static_cast<uint32_t>(
                    validationLayers.size());
            createInfo.ppEnabledLayerNames =
                validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo,
                           nullptr,
                           &device) != VK_SUCCESS) {
            throw std::runtime_error(
                "failed to create logical device");
        }

        vkGetDeviceQueue(device,
                         indices.graphicsFamily.value(), 0,
                         &graphicsQueue);
        vkGetDeviceQueue(device,
                         indices.presentFamily.value(), 0,
                         &presentQueue);
    }

    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount,
                                   nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error(
                "failed to find GPUs with with Vulkan "
                "support");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount,
                                   devices.data());

        for (auto device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error(
                "failed to find suitable GPU");
        }
    }

    bool isDeviceSuitable(VkPhysicalDevice device) {
        // TODO: add a ranking mechanism to pick the best
        // GPU VkPhysicalDeviceProperties deviceProperties;
        // vkGetPhysicalDeviceProperties(device,
        // &deviceProperties);
        //
        // VkPhysicalDeviceFeatures deviceFeatures;
        // vkGetPhysicalDeviceFeatures(device,
        // &deviceFeatures);

        QueueFamilyIndices indices =
            findQueueFamilies(device, surface);
        return indices.isComplete();

        // #ifdef __APPLE__
        //         return true;
        // #else
        //         return deviceProperties.deviceType ==
        //         VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        // #endif
    }

    bool checkValidationLayerSupport() {
        uint32_t layerCount;

        vkEnumerateInstanceLayerProperties(&layerCount,
                                           nullptr);

        std::vector<VkLayerProperties> availableLayers(
            layerCount);
        vkEnumerateInstanceLayerProperties(
            &layerCount, availableLayers.data());

        for (auto layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties :
                 availableLayers) {
                if (strcmp(layerName,
                           layerProperties.layerName) ==
                    0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT
            messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT*
            pCallbackData,
        void* pUserData) {
        std::cerr << "validation layer: "
                  << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    void populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType =
            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;  // optional
    }

    void setupDebugMessenger() {
        if (!enableValidationLayers) {
            return;
        }

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populateDebugMessengerCreateInfo(createInfo);

        if (createDebugUtilsMessengerEXT(
                instance, &createInfo, nullptr,
                &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error(
                "failed to set up debug messenger");
        }
    }

    void printAvaliableExtensions() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(
            nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(
            extensionCount);
        vkEnumerateInstanceExtensionProperties(
            nullptr, &extensionCount, extensions.data());

        std::cout << "avaliable extensions: \n";

        for (auto extension : extensions) {
            std::cout << '\t' << extension.extensionName
                      << '\n';
        }
    }

    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(
            &glfwExtensionCount);

        // this is an iterator
        std::vector<const char*> extensions(
            glfwExtensions,
            glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

#ifdef __APPLE__
        extensions.push_back(
            VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
        return extensions;
    }

    void createInstance() {
        if (enableValidationLayers &&
            !checkValidationLayerSupport()) {
            throw std::runtime_error(
                "validation layers requested, but not "
                "avaliable");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "vktest";
        appInfo.applicationVersion =
            VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Min Jaf";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType =
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        printAvaliableExtensions();

        std::vector<const char*> requiredExtensions =
            getRequiredExtensions();

#ifdef __APPLE__
        createInfo.flags |=
            VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

        createInfo.enabledExtensionCount =
            static_cast<uint32_t>(
                requiredExtensions.size());
        createInfo.ppEnabledExtensionNames =
            requiredExtensions.data();

        if (enableValidationLayers) {
            VkDebugUtilsMessengerCreateInfoEXT
                debugCreateInfo{};
            populateDebugMessengerCreateInfo(
                debugCreateInfo);

            createInfo.enabledLayerCount =
                static_cast<uint32_t>(
                    validationLayers.size());
            createInfo.ppEnabledLayerNames =
                validationLayers.data();
            createInfo.pNext =
                (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr,
                             &instance) != VK_SUCCESS) {
            throw std::runtime_error(
                "failed to create instance");
        }
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            if (gSignalStatus == SIGINT) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
        }
    }

    void cleanup() {
        vkDestroyDevice(device, nullptr);

        if (enableValidationLayers) {
            destroyDebugUtilsMessengerEXT(
                instance, debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    GLFWwindow* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
};

int main() {
    Application app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
