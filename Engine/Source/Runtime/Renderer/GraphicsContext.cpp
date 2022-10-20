#include "Xpch.h"

#include "Runtime/Renderer/GraphicsContext.h"
#include "Runtime/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VulkanContext.h"

namespace X 
{
	Ref<GraphicsContext> GraphicsContext::Create(void* window) 
	{
		switch (RendererAPI::Current()) 
		{
			case RendererAPI::RendererAPIType::None:    X_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::RendererAPIType::OpenGL:  return CreateRef<OpenGLContext>(static_cast<GLFWwindow*>(window));
			case RendererAPI::RendererAPIType::Vulkan:  return CreateRef<VulkanContext>(static_cast<GLFWwindow*>(window)); 
			case RendererAPI::RendererAPIType::DX11:    return nullptr;
			case RendererAPI::RendererAPIType::DX12:    return nullptr;
		}

		X_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}