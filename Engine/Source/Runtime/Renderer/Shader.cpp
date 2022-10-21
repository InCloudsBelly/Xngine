#include "Xpch.h"

#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/NativeOpenGLShader.h"

namespace X
{

    Ref<Shader> Shader::Create(const std::filesystem::path& filepath)
    {
        return Create(filepath.string());
    }

    Ref<Shader> Shader::Create(const std::string& filepath)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::RendererAPIType::None:    X_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::RendererAPIType::OpenGL:  return std::make_shared<OpenGLShader>(filepath);
        case RendererAPI::RendererAPIType::Vulkan:  return nullptr;
        case RendererAPI::RendererAPIType::DX11:    return nullptr;
        case RendererAPI::RendererAPIType::DX12:    return nullptr;
        }

        X_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::RendererAPIType::None:    X_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::RendererAPIType::OpenGL:  return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
        case RendererAPI::RendererAPIType::Vulkan:  return nullptr;
        case RendererAPI::RendererAPIType::DX11:    return nullptr;
        case RendererAPI::RendererAPIType::DX12:    return nullptr;
        }

        X_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Shader> Shader::CreateNative(const std::filesystem::path& filepath)
    {
        return CreateNative(filepath.string());
    }

    Ref<Shader> Shader::CreateNative(const std::string& filepath)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::RendererAPIType::None:    X_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::RendererAPIType::OpenGL:  return std::make_shared<NativeOpenGLShader>(filepath);
        case RendererAPI::RendererAPIType::Vulkan:  return nullptr;
        case RendererAPI::RendererAPIType::DX11:    return nullptr;
        case RendererAPI::RendererAPIType::DX12:    return nullptr;
        }

        X_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Shader> Shader::CreateNative(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::RendererAPIType::None:    X_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::RendererAPIType::OpenGL:  return std::make_shared<NativeOpenGLShader>(name, vertexSrc, fragmentSrc);
        case RendererAPI::RendererAPIType::Vulkan:  return nullptr;
        case RendererAPI::RendererAPIType::DX11:    return nullptr;
        case RendererAPI::RendererAPIType::DX12:    return nullptr;
        }

        X_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    ShaderUniform::ShaderUniform(const std::string& name, ShaderUniformType type, uint32_t size, uint32_t offset)
        : mName(name), mType(type), mSize(size), mOffset(offset)
    {
    }

    const std::string& ShaderUniform::UniformTypeToString(ShaderUniformType type)
    {
        if (type == ShaderUniformType::Bool)
        {
            return "Boolean";
        }
        else if (type == ShaderUniformType::Int)
        {
            return "Int";
        }
        else if (type == ShaderUniformType::Float)
        {
            return "Float";
        }

        return "None";
    }
}