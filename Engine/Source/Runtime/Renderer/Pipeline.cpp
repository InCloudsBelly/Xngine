
#include "Xpch.h"

#include "Runtime/Platform/OpenGL/OpenGLPipeline.h"

#include "Runtime/Renderer/Pipeline.h"
#include "Runtime/Renderer/RendererAPI.h"

namespace X
{
	Ref<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPI::RendererAPIType::None:   return Ref<Pipeline>();
		case RendererAPI::RendererAPIType::OpenGL: return CreateRef<OpenGLPipeline>(spec);
		case RendererAPI::RendererAPIType::Vulkan: return nullptr;
		case RendererAPI::RendererAPIType::DX11:   return nullptr;
		}

		X_CORE_ASSERT(false, "Unknown RendererAPI");
		return Ref<Pipeline>();
	}
}