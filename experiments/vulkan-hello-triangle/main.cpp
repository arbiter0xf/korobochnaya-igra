#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

const std::vector< const char*> g_validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifndef NDEBUG
const bool g_enableValidationLayers = true;
#endif
#ifdef NDEBUG
const bool g_enableValidationLayers = false;
#endif

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

bool
supportForLayer(
        const char* requestedLayer,
        std::vector<VkLayerProperties>& availableLayers)
{
    uint32_t ret;
    size_t layerNameMaxLen;

    ret = -1;
    layerNameMaxLen = 128;

    for (const VkLayerProperties& available : availableLayers) {
        ret = strncmp(requestedLayer, available.layerName, layerNameMaxLen);
        if (0 == ret) {
            return true;
        }
    }

    return false;
}

bool
checkValidationLayerSupport()
{
    bool haveSupport;
    uint32_t layerCount;

    std::vector<VkLayerProperties> availableLayers;

    layerCount = 0;
    haveSupport = false;

    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    availableLayers.resize(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* requestedLayer : g_validationLayers) {
        haveSupport = supportForLayer(requestedLayer, availableLayers);
        if ( ! haveSupport) {
            std::cerr
                << "Missing support for validation layer: "
                << requestedLayer
                << '\n';

            return false;
        }
    }

    return true;
}

void
createVulkanInstance()
{
    const std::string applicationName = "Hello Triangle";
    const std::string engineName = "No engine";

    uint32_t fullExtensionCount; // Set to amount of all supported extensions
    uint32_t glfwExtensionCount;
    const char** glfwExtensions;

    bool haveSupport;

    std::string msg;
    std::vector<VkExtensionProperties> extensions;

    VkResult result;
    VkApplicationInfo appInfo;
    VkInstanceCreateInfo createInfo;

    msg = "";
    result = VK_ERROR_UNKNOWN;
    appInfo = {};
    createInfo = {};
    haveSupport = false;
    glfwExtensions = 0;
    glfwExtensionCount = 0;
    fullExtensionCount = 0;

    if (g_enableValidationLayers) {
        haveSupport = checkValidationLayerSupport();
        if ( ! haveSupport) {
            throw std::runtime_error("[!] Validation layers requested, but not available.");
        }
    }

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

    createInfo.enabledLayerCount = 0; // Default to no validation layers
    if (g_enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(g_validationLayers.size());
        createInfo.ppEnabledLayerNames = g_validationLayers.data();
    }

    vkEnumerateInstanceExtensionProperties(nullptr, &fullExtensionCount, nullptr);
    extensions.resize(fullExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &fullExtensionCount, extensions.data());

    std::cout << "Enumerated available extensions. Found:\n";
    for (const VkExtensionProperties& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }

    result = vkCreateInstance(&createInfo, nullptr, &vulkanInstance);
    if (VK_SUCCESS != result) {
        msg = "[!] Failed to create Vulkan instance. (";
        msg += result;
        msg += ")";
        throw std::runtime_error(msg.c_str());
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
