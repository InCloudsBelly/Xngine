#include "Xpch.h"

#include "Runtime/Platform/Vulkan/VulkanContext.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace X
{
    VulkanContext::VulkanContext(GLFWwindow* windowHandle)
        : mWindowHandle(windowHandle)
    {
        X_CORE_ASSERT(windowHandle, "Window handle is null!");
    }

    void VulkanContext::Init()
    {
        glfwMakeContextCurrent(mWindowHandle);
    }

    void VulkanContext::SwapBuffers()
    {
        X_PROFILE_FUNCTION();

        glfwSwapBuffers(mWindowHandle);
    }
}