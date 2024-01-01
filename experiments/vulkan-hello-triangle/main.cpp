#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

class HelloTriangleApplication {

public:

void
run()
{
    initWindow();
    initVulkan();
    mainLoop();

    std::cout << "Shutting down." << '\n';

    cleanup();
}

private:

GLFWwindow* window;
VkInstance vulkanInstance;

void
initWindow()
{
    const std::string WIN_TITLE  = "Vulkan";
    const uint32_t    WIN_WIDTH  = 800;
    const uint32_t    WIN_HEIGHT = 600;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(
            WIN_WIDTH,
            WIN_HEIGHT,
            WIN_TITLE.c_str(),
            nullptr,
            nullptr);
}

void
createVulkanInstance()
{
    const std::string applicationName = "Hello Triangle";
    const std::string engineName = "No engine";

    uint32_t fullExtensionCount; // Set to amount of all supported extensions
    uint32_t glfwExtensionCount;
    const char** glfwExtensions;

    std::vector<VkExtensionProperties> extensions;

    VkResult result;
    VkApplicationInfo appInfo;
    VkInstanceCreateInfo createInfo;

    result = VK_ERROR_UNKNOWN;
    appInfo = {};
    createInfo = {};
    glfwExtensions = 0;
    glfwExtensionCount = 0;
    fullExtensionCount = 0;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = applicationName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = engineName.c_str();
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;

    vkEnumerateInstanceExtensionProperties(nullptr, &fullExtensionCount, nullptr);
    extensions.resize(fullExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &fullExtensionCount, extensions.data());

    std::cout << "Enumerated available extensions. Found:\n";
    for (const VkExtensionProperties& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }

    result = vkCreateInstance(&createInfo, nullptr, &vulkanInstance);
    if (VK_SUCCESS != result) {
        throw std::runtime_error("Failed to create Vulkan instance.");
    }

    std::cout << "Vulkan instance created." << std::endl;
}

void
initVulkan()
{
    createVulkanInstance();
}

void
mainLoop()
{
    while ( ! glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void
cleanup()
{
    vkDestroyInstance(vulkanInstance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();

    std::cout << "Cleanup done!" << '\n';
}

}; // class HelloTriangleApplication

int main(void) {
    HelloTriangleApplication application;

    try {
        application.run();
    } catch(const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
