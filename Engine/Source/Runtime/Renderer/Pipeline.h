#define _CRT_SECURE_NO_WARNINGS
#pragma once

#include "Runtime/Core/Base/Base.h"


#include "Shader.h"
#include "VertexBuffer.h"
#include "RenderPass.h"


namespace X
{
	enum class PrimitiveTopology
	{
		None = 0,
		Points,
		Lines,
		Triangles,
		LineStrip,
		TriangleStrip,
		TriangleFan
	};
	enum class DepthCompareOperator
	{
		None = 0,
		Never,
		NotEqual,
		Less,
		LessOrEqual,
		Greater,
		GreaterOrEqual,
		Equal,
		Always,
	};

	struct PipelineSpecification
	{
		Ref<Shader> Shader;
		BufferLayout StaticLayout;
		BufferLayout AnimationLayout;
		Ref<RenderPass> RenderPass;

		PrimitiveTopology Topology = PrimitiveTopology::Triangles;
		DepthCompareOperator DepthOperator = DepthCompareOperator::GreaterOrEqual;
		bool BackfaceCulling = true;
		bool DepthTest = true;
		bool DepthWrite = true;
		bool Wireframe = false;
		float LineWidth = 1.0f;

		std::string DebugName;
	};

	struct PipelineStatistics
	{
		uint64_t InputAssemblyVertices = 0;
		uint64_t InputAssemblyPrimitives = 0;
		uint64_t VertexShaderInvocations = 0;
		uint64_t ClippingInvocations = 0;
		uint64_t ClippingPrimitives = 0;
		uint64_t FragmentShaderInvocations = 0;
		uint64_t ComputeShaderInvocations = 0;

		// TODO: tesselation shader stats when we have them
	};

	class Pipeline 
	{
	public:
		virtual ~Pipeline() = default;

		virtual PipelineSpecification& GetSpecification() = 0;
		virtual const PipelineSpecification& GetSpecification() const = 0;

		virtual void Invalidate() = 0;

		// TEMP: remove this when render command buffers are a thing
		virtual void BindStatic() = 0;
		virtual void BindAnimation() = 0;

		virtual void Unbind() = 0;

		static Ref<Pipeline> Create(const PipelineSpecification& spec);

	};
}
