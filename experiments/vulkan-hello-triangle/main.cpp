#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

class HelloTriangleApplication {

public:

void
run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

private:

GLFWwindow* window;

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
initVulkan()
{
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
    glfwDestroyWindow(window);

    glfwTerminate();
}

};

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
