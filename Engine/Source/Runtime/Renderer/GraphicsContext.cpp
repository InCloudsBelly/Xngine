#include "Xpch.h"

#include "Runtime/Renderer/GraphicsContext.h"
#include "Runtime/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace X 
{
	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (RendererAPI::Current()) 
		{
			case RendererAPI::RendererAPIType::None:    X_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::RendererAPIType::OpenGL:  return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
			//case RendererAPI::RendererAPIType::Vulkan:  return CreateScope<VulkanContext>(static_cast<GLFWwindow*>(window));
		}

		X_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}