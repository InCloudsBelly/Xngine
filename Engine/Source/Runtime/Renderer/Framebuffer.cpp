#include "Xpch.h"
#include "Framebuffer.h"

#include "Runtime/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace X
{
    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:    X_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return CreateRef<OpenGLFramebuffer>(spec);
        }
        X_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}