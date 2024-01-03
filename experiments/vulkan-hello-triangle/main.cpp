#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cassert>
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

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
{
    std::cerr << "Validation layer: " << pCallbackData->pMessage << '\n';

    // Returning VK_TRUE aborts the call that triggered the validation layer
    // message. This behavior is mostly used for testing validation layers
    // themselves, so should always return VK_FALSE.
    return VK_FALSE;
}

/*
 * Supposedly sets a messenger to parameter 'pDebugMessenger'.
 * Returns whether successful or not.
 */
static VkResult
createDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    VkResult result;
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    result = VK_ERROR_UNKNOWN;

    if (nullptr == func) {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    result = func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    return result;
}

static void
destroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (nullptr != func) {
        func(instance, debugMessenger, pAllocator);
    }
}

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
VkDebugUtilsMessengerEXT debugMessenger;

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

std::vector<const char*>
getRequiredExtensions()
{
    const char** glfwExtensions;

    uint32_t glfwExtensionCount;

    std::vector<const char*> extensions;

    extensions = {};
    glfwExtensions = 0;
    glfwExtensionCount = 0;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        extensions.push_back( *(glfwExtensions + i) );
    }

    assert(extensions.size() == glfwExtensionCount);

    if (g_enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void
createVulkanInstance()
{
    const std::string applicationName = "Hello Triangle";
    const std::string engineName = "No engine";

    uint32_t fullExtensionCount; // Set to amount of all supported extensions

    bool haveSupport;

    std::string msg;
    std::vector<const char*> extensionsToRequest;

    // Used for querying available extensions
    std::vector<VkExtensionProperties> extensions;

    VkResult result;
    VkApplicationInfo appInfo;
    VkInstanceCreateInfo createInfo;

    msg = "";
    result = VK_ERROR_UNKNOWN;
    appInfo = {};
    createInfo = {};
    haveSupport = false;
    fullExtensionCount = 0;
    extensionsToRequest = {};

    if (g_enableValidationLayers) {
        haveSupport = checkValidationLayerSupport();
        if ( ! haveSupport) {
            throw std::runtime_error(
                    "[!] Validation layers requested, but not available.");
        }
    }

    extensionsToRequest = getRequiredExtensions();

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = applicationName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = engineName.c_str();
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(
            extensionsToRequest.size());
    createInfo.ppEnabledExtensionNames = extensionsToRequest.data();

    createInfo.enabledLayerCount = 0; // Default to no validation layers
    if (g_enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(
                g_validationLayers.size());
        createInfo.ppEnabledLayerNames = g_validationLayers.data();
    }

    vkEnumerateInstanceExtensionProperties(
            nullptr,
            &fullExtensionCount,
            nullptr);
    extensions.resize(fullExtensionCount);

    vkEnumerateInstanceExtensionProperties(
            nullptr,
            &fullExtensionCount,
            extensions.data());

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

VkDebugUtilsMessengerCreateInfoEXT
getDebugMessengerCreateInfo()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo;

    createInfo = {};

    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional

    return createInfo;
}

void
setupDebugMessenger()
{
    VkResult result;
    VkDebugUtilsMessengerCreateInfoEXT createInfo;

    if ( ! g_enableValidationLayers) {
        return;
    }

    createInfo = {};
    result = VK_ERROR_UNKNOWN;

    createInfo = getDebugMessengerCreateInfo();

    result = createDebugUtilsMessengerEXT(
            vulkanInstance,
            &createInfo,
            nullptr,
            &debugMessenger);
    if (VK_SUCCESS != result) {
        std::runtime_error("[!] Failed to set up debug messenger.");
    }
}

void
teardownDebugMessenger()
{
    if ( ! g_enableValidationLayers) {
        return;
    }

    destroyDebugUtilsMessengerEXT(
            vulkanInstance,
            debugMessenger,
            nullptr);
}

void
initVulkan()
{
    createVulkanInstance();
    setupDebugMessenger();
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
    teardownDebugMessenger();

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
