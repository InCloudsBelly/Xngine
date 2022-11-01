#include "Xpch.h"
#include "Runtime/Renderer/RenderPass.h"
#include "Runtime/Renderer/Renderer.h"
#include "Runtime/Platform/OpenGL/OpenGLRenderPass.h"

namespace X
{

    Ref<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::RendererAPIType::None:    X_CORE_ASSERT(false, "RendererAPI_::None is currently not supported!"); return Ref<RenderPass>();
        case RendererAPI::RendererAPIType::OpenGL:  return CreateRef<OpenGLRenderPass>(spec);
        case RendererAPI::RendererAPIType::Vulkan:  return nullptr;
        case RendererAPI::RendererAPIType::DX11:    return nullptr;
        }

        X_CORE_ASSERT(false, "Unknown RendererAPI!");
        return Ref<RenderPass>();
    }

}