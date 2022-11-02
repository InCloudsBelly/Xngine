
#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "Runtime/Renderer/Pipeline.h"


namespace X
{

	class OpenGLPipeline : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification& spec);
		virtual ~OpenGLPipeline();

		virtual PipelineSpecification& GetSpecification() { return mSpecification; }
		virtual const PipelineSpecification& GetSpecification() const { return mSpecification; }

		virtual void Invalidate() override;

		// TEMP: remove this when render command buffers are a thing
		virtual void BindStatic() override;
		virtual void BindAnimation() override;

		virtual void Unbind() override;
	private:
		PipelineSpecification mSpecification;
		uint32_t mVertexArrayRendererID = 0;

	};

}
