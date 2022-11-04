#pragma once
#include "Runtime/Renderer/Framebuffer.h"
#include "Runtime/Renderer/VertexArray.h"
#include "Runtime/Renderer/Texture.h"

namespace X
{
	enum class PostProcessingType
	{
		None = 1,
		MSAA,
		Outline,
		Cartoon,
		GrayScale,
		GaussianBlur,
		ComputeTest,
	};

	class PostProcessing
	{
	public:
		PostProcessing(const PostProcessingType& type);

		virtual ~PostProcessing() {}
	public:
		static void Init();
		virtual uint32_t ExcuteAndReturnFinalTex(const Ref<Framebuffer>& fb, const uint32_t& colorIndex = 0) { return 0; };

		static std::string PostTypeToString(PostProcessingType type);

		static void ResetPostProcessing();
	protected:
		void DoPostProcessing();
	public:
		PostProcessingType mType;
		
		static bool mAlreadyProcessed;
		static Ref<Framebuffer> mFramebuffer;
		static Ref<Texture2D> mIntermediateScreenTex;
	protected:
		static Ref<VertexArray> mScreenQuadVAO;
		static Ref<VertexBuffer> mScreenQuadVBO;
		static Ref<IndexBuffer> mScreenQuadIBO;
	};
}