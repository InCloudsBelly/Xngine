#pragma once

#include "Runtime/Renderer/RenderPass.h"

namespace X
{
	class OpenGLRenderPass : public RenderPass
	{
	public:
		OpenGLRenderPass(const RenderPassSpecification& spec)
		: mSpecification(spec) {}

		virtual ~OpenGLRenderPass() {}

		virtual RenderPassSpecification& GetSpecification() override { return mSpecification; }
		virtual const RenderPassSpecification& GetSpecification() const override { return mSpecification; }

		virtual std::vector<Scope<PostProcessing>>& GetPostProcessings() override { return mPostProcessings; }
		virtual const std::vector<Scope<PostProcessing>>& GetPostProcessings() const override { return mPostProcessings; }

		virtual void AddPostProcessing(PostProcessingType type) override;

		virtual uint32_t ExcuteAndReturnFinalTex() override;

	private:
		RenderPassSpecification mSpecification;

		std::vector<Scope<PostProcessing>> mPostProcessings;
	};

}