#include "Xpch.h"
#include "Runtime/Renderer/Texture.h"

#include "Runtime/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace X
{
    Ref<Texture2D> Texture2D::Create(const std::filesystem::path& path)
    {
        return Create(path.string());
    }

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:    X_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(width, height);
        }

        X_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Texture2D> Texture2D::Create(const std::string& path)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:    X_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path);
        }

        X_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
