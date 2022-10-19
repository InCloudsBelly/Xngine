#include "Xpch.h"

#include "Runtime/Renderer/UniformBuffer.h"
#include "Runtime/Renderer/Renderer.h"
#include "Runtime/Platform/OpenGL/OpenGLUniformBuffer.h"

namespace X
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPI::RendererAPIType::None:    X_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::RendererAPIType::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		X_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}