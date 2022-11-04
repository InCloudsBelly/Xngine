#pragma once

#include "Runtime/Renderer/PostProcessing.h"

namespace X
{
	class OpenGLPostProcessing : public PostProcessing
	{
	public:
		OpenGLPostProcessing(const PostProcessingType& type) : PostProcessing(type) {}
	public:
		virtual uint32_t ExcuteAndReturnFinalTex(const Ref<Framebuffer>& fb, const uint32_t& colorIndex = 0) override;

	private:
		uint32_t DoMSAA(const Ref<Framebuffer>& fb, const uint32_t& colorIndex);
		uint32_t DoPostWithShader(const Ref<Framebuffer>& fb, const Ref<class Shader>& shader, const uint32_t& colorIndex);
		uint32_t DoPostWithComputeShader(const Ref<Framebuffer>& fb, const Ref<class Shader>& shader, const uint32_t& colorIndex);
	};
}