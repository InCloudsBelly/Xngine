#pragma once
#include "Runtime/Renderer/Framebuffer.h"
#include "Runtime/Renderer/PostProcessing.h"

namespace X
{
	struct  RenderPassSpecification
	{
		Ref<Framebuffer> TargetFramebuffer;
		std::string DebugName;

	};

	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;

		virtual RenderPassSpecification& GetSpecification() = 0; 
		virtual const RenderPassSpecification& GetSpecification() const = 0; 

		virtual std::vector<Scope<PostProcessing>>& GetPostProcessings() = 0;
		virtual const std::vector<Scope<PostProcessing>>& GetPostProcessings() const = 0;

		virtual void AddPostProcessing(PostProcessingType type) = 0;

		virtual uint32_t ExcuteAndReturnFinalTex() = 0;


		static Ref<RenderPass> Create(const RenderPassSpecification& spec);
	};
}