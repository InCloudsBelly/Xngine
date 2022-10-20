#pragma once
#include "Runtime/Renderer/Framebuffer.h"
#include "Runtime/Renderer/PostProcessing.h"

namespace X
{
	struct  RenderPassSpecification
	{
		Ref<Framebuffer> TargetFramebuffer = nullptr;
		std::string DebugName = "";

	};

	class RenderPass
	{
	public:
		RenderPass(RenderPassSpecification Spec) : mSpecification(Spec) {};
		virtual ~RenderPass() = default;

	public:
		RenderPassSpecification& GetSpecification() { return mSpecification; };
		const RenderPassSpecification& GetSpecification() const { return mSpecification; };

		void AddPostProcessing(PostProcessingType type);
		uint32_t ExcuteAndReturnFinalTex()
		{
			uint32_t final = 0;
			for (auto& effect : mPostProcessings)
			{
				final = effect->ExcuteAndReturnFinalTex(mSpecification.TargetFramebuffer);
			}
			return final;
		}

	private:
		RenderPassSpecification mSpecification;
		std::vector<Scope<PostProcessing>> mPostProcessings;
	};
}