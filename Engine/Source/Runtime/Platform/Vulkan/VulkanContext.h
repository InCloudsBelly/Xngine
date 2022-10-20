#pragma once

#include "Runtime/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace X
{
    class VulkanContext : public GraphicsContext
    {
    public:
        VulkanContext(GLFWwindow* windowHandle);

        virtual void Init() override;
        virtual void SwapBuffers() override;
    private:
        GLFWwindow* mWindowHandle;
    };
}