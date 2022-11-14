#include "Xpch.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"
#include "Runtime/Renderer/Renderer3D.h"
#include "Runtime/Renderer/Texture.h"
#include "Runtime/Renderer/VertexArray.h"
#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/RenderCommand.h"
#include "Runtime/Renderer/UniformBuffer.h"
#include "Runtime/Renderer/Framebuffer.h"
#include "Runtime/Library/ShaderLibrary.h"
#include "Runtime/Library/UniformBufferLibrary.h"
#include "Runtime/Library/Library.h"

#include "Runtime/Resource/AssetManager/AssetManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

namespace X
{
	Ref<Pipeline> Renderer3D::HBAOGbufferPipeline = nullptr;
	Ref<Texture2D> Renderer3D::TextureConfigNoise = nullptr;
	Ref<Pipeline> Renderer3D::HBAOPipeline = nullptr;
	Ref<Pipeline> Renderer3D::HBAOBlurPipeline = nullptr;

	Ref<Pipeline> Renderer3D::lightPipeline = nullptr;
	Ref<Pipeline> Renderer3D::GeometryPipeline = nullptr;
	Ref<Pipeline> Renderer3D::QuadPipeline = nullptr;
	Ref<Pipeline> Renderer3D::HBAOQuadPipeline = nullptr;

	float* generateNoise()
	{
		float* noise = new float[4 * 4 * 4];
		for (int y = 0; y < 4; ++y)
		{
			for (int x = 0; x < 4; ++x)
			{
				glm::vec2 xy = glm::circularRand(1.0f);
				float z = glm::linearRand(0.0f, 1.0f);
				float w = glm::linearRand(0.0f, 1.0f);

				int offset = 4 * (y * 4 + x);
				noise[offset + 0] = xy[0];
				noise[offset + 1] = xy[1];
				noise[offset + 2] = z;
				noise[offset + 3] = w;
			}
		}
		return noise;
	}

	void Renderer3D::Init()
	{
		// HBAO 
		{
			//Gbuffer pass
			{
				FramebufferSpecification fbSpec;
				fbSpec.Attachments = { FramebufferTextureFormat::RGBA8,FramebufferTextureFormat::RGBA8 ,FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::DEPTH32F_TEX2D };
				fbSpec.Width = 1280;
				fbSpec.Height = 720;
				fbSpec.Samples = 1;
				Ref<Framebuffer> GbufferFB = Framebuffer::Create(fbSpec);

				RenderPassSpecification GbufferPassSpec;
				GbufferPassSpec.DebugName = "HBAOGbufferPass";
				GbufferPassSpec.TargetFramebuffer = GbufferFB;

				PipelineSpecification pipelineSpec;
				pipelineSpec.DebugName = GbufferPassSpec.DebugName;
				pipelineSpec.Shader = Library<Shader>::GetInstance().Get("HBAOGbuffer");
				pipelineSpec.RenderPass = RenderPass::Create(GbufferPassSpec);

				pipelineSpec.StaticLayout = {
					{ ShaderDataType::Float3, "a_Pos"},
					{ ShaderDataType::Float3, "a_Normal"},
					{ ShaderDataType::Float2, "a_TexCoord"},
					{ ShaderDataType::Float3, "a_Tangent"},
					{ ShaderDataType::Float3, "a_Bitangent"},
					{ ShaderDataType::Int,	  "a_EntityID"},
				};

				pipelineSpec.AnimationLayout = {
					{ ShaderDataType::Float3, "a_Pos"},
					{ ShaderDataType::Float2, "a_TexCoord"},
					{ ShaderDataType::Float3, "a_Tangent"},
					{ ShaderDataType::Float3, "a_Bitangent"},
					{ ShaderDataType::Int,	  "a_EntityID"},
					{ ShaderDataType::Int4,   "a_BoneIDs"},
					{ ShaderDataType::Float4, "a_Weights"},
				};

				HBAOGbufferPipeline = Pipeline::Create(pipelineSpec);
			}

			//HBAO pass
			{
				FramebufferSpecification fbSpec;
				fbSpec.Attachments = { FramebufferTextureFormat::R8 };
				fbSpec.Width = 1280;
				fbSpec.Height = 720;
				Ref<Framebuffer> HBAOFramebuffer = Framebuffer::Create(fbSpec);



				TextureConfigNoise = Texture2D::Create(4, 4, DataFormat::RGBA16F, DataFormat::RGBA, DataFormat::RGBA,
					FilterType::Nearest, FilterType::Nearest, WrapType::Repeat, WrapType::Repeat);
				float* HBAONoise = generateNoise();
				TextureConfigNoise->SetData(HBAONoise, 4 * 4 * 4, DataFormat::FLOAT);

				RenderPassSpecification HBAOSpec = { HBAOFramebuffer, "HBAO" };

				PipelineSpecification pipelineSpec;
				pipelineSpec.DebugName = HBAOSpec.DebugName;
				pipelineSpec.Shader = Library<Shader>::GetInstance().Get("HBAO");


				pipelineSpec.RenderPass = RenderPass::Create(HBAOSpec);

				pipelineSpec.StaticLayout = {
					{ ShaderDataType::Float3, "a_Pos"},
				};

				pipelineSpec.AnimationLayout = {
					{ ShaderDataType::Float3, "a_Pos"},
				};

				HBAOPipeline = Pipeline::Create(pipelineSpec);
			}


			//HBAO blur pass
			{
				FramebufferSpecification fbSpec;
				fbSpec.Attachments = { FramebufferTextureFormat::R8 };
				fbSpec.Width = 1280;
				fbSpec.Height = 720;
				Ref<Framebuffer> HBAOFramebuffer = Framebuffer::Create(fbSpec);

				RenderPassSpecification HBAOBlueSpec = { HBAOFramebuffer, "HBAOBlur" };

				PipelineSpecification pipelineSpec;
				pipelineSpec.DebugName = HBAOBlueSpec.DebugName;
				pipelineSpec.Shader = Library<Shader>::GetInstance().Get("HBAOBlur");


				pipelineSpec.RenderPass = RenderPass::Create(HBAOBlueSpec);

				pipelineSpec.StaticLayout = {
					{ ShaderDataType::Float3, "a_Pos"},
				};

				pipelineSpec.AnimationLayout = {
					{ ShaderDataType::Float3, "a_Pos"},
				};

				HBAOBlurPipeline = Pipeline::Create(pipelineSpec);
			}
			

			//QuadPipeline
			{
				FramebufferSpecification fbSpec;
				fbSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
				fbSpec.Width = 1280;
				fbSpec.Height = 720;
				Ref<Framebuffer> GeoFramebuffer = Framebuffer::Create(fbSpec);

				RenderPassSpecification QuadSpec = { GeoFramebuffer, "HBAOQuad" };

				PipelineSpecification pipelineSpec;
				pipelineSpec.DebugName = QuadSpec.DebugName;
				pipelineSpec.Shader = Library<Shader>::GetInstance().Get("HBAOQuad");
				pipelineSpec.RenderPass = RenderPass::Create(QuadSpec);

				pipelineSpec.StaticLayout = {
					{ ShaderDataType::Float3, "a_Pos"},
				};

				pipelineSpec.AnimationLayout = {
					{ ShaderDataType::Float3, "a_Pos"},
				};

				HBAOQuadPipeline = Pipeline::Create(pipelineSpec);
			}

		}


		//Directional light shadow pass
		{
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = { FramebufferTextureFormat::DEPTH32F_TEX3D };
			// light depth texture uses high resolution
			fbSpec.Width = 2048;
			fbSpec.Height = 2048;

			Ref<Framebuffer> lightFBO = Framebuffer::Create(fbSpec);

			RenderPassSpecification ShadowPassSpec;
			ShadowPassSpec.DebugName = "ShadowPass";
			ShadowPassSpec.TargetFramebuffer = lightFBO;

			PipelineSpecification pipelineSpec;
			pipelineSpec.DebugName = ShadowPassSpec.DebugName;
			pipelineSpec.Shader = Library<Shader>::GetInstance().Get("CSM_Depth");

			pipelineSpec.StaticLayout = {
				{ ShaderDataType::Float3, "a_Pos"},
				{ ShaderDataType::Float3, "a_Normal"},
				{ ShaderDataType::Float2, "a_TexCoord"},
				{ ShaderDataType::Float3, "a_Tangent"},
				{ ShaderDataType::Float3, "a_Bitangent"},
				{ ShaderDataType::Int,	  "a_EntityID"},
			};

			pipelineSpec.AnimationLayout = {
				{ ShaderDataType::Float3, "a_Pos"},
				{ ShaderDataType::Float3, "a_Normal"},
				{ ShaderDataType::Float2, "a_TexCoord"},
				{ ShaderDataType::Float3, "a_Tangent"},
				{ ShaderDataType::Float3, "a_Bitangent"},
				{ ShaderDataType::Int,	  "a_EntityID"},
				{ ShaderDataType::Int4,   "a_BoneIDs"},
				{ ShaderDataType::Float4, "a_Weights"},
			};

			pipelineSpec.RenderPass = RenderPass::Create(ShadowPassSpec);
			lightPipeline = Pipeline::Create(pipelineSpec);
		}


		//GeometryPipeline
		{
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::R8,  FramebufferTextureFormat::DEPTH24STENCIL8 };
			fbSpec.Width = 1280;
			fbSpec.Height = 720;
			fbSpec.Samples = 4;
			Ref<Framebuffer> GeoFramebuffer = Framebuffer::Create(fbSpec);

			RenderPassSpecification geoSpec = { GeoFramebuffer, "MainPass" };

			PipelineSpecification pipelineSpec;
			pipelineSpec.DebugName = geoSpec.DebugName;
			pipelineSpec.Shader = Library<Shader>::GetInstance().Get("BasePBR");
			pipelineSpec.RenderPass = RenderPass::Create(geoSpec);

			if (pipelineSpec.RenderPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples > 1)
				pipelineSpec.RenderPass->AddPostProcessing(PostProcessingType::MSAA);

			pipelineSpec.StaticLayout = {
				{ ShaderDataType::Float3, "a_Pos"},
				{ ShaderDataType::Float3, "a_Normal"},
				{ ShaderDataType::Float2, "a_TexCoord"},
				{ ShaderDataType::Float3, "a_Tangent"},
				{ ShaderDataType::Float3, "a_Bitangent"},
				{ ShaderDataType::Int,	  "a_EntityID"},
			};

			pipelineSpec.AnimationLayout = {
				{ ShaderDataType::Float3, "a_Pos"},
				{ ShaderDataType::Float3, "a_Normal"},
				{ ShaderDataType::Float2, "a_TexCoord"},
				{ ShaderDataType::Float3, "a_Tangent"},
				{ ShaderDataType::Float3, "a_Bitangent"},
				{ ShaderDataType::Int,	  "a_EntityID"},
				{ ShaderDataType::Int4,   "a_BoneIDs"},
				{ ShaderDataType::Float4, "a_Weights"},
			};

			GeometryPipeline = Pipeline::Create(pipelineSpec);
		}

		//QuadPipeline
		{
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
			fbSpec.Width = 1280;
			fbSpec.Height = 720;
			Ref<Framebuffer> GeoFramebuffer = Framebuffer::Create(fbSpec);

			RenderPassSpecification QuadSpec = { GeoFramebuffer, "SimpleQuad" };

			PipelineSpecification pipelineSpec;
			pipelineSpec.DebugName = QuadSpec.DebugName;
			pipelineSpec.Shader = Library<Shader>::GetInstance().Get("SimpleQuad");
			pipelineSpec.RenderPass = RenderPass::Create(QuadSpec);

			pipelineSpec.StaticLayout = {
				{ ShaderDataType::Float3, "a_Pos"},
			};

			pipelineSpec.AnimationLayout = {
				{ ShaderDataType::Float3, "a_Pos"},
			};

			QuadPipeline = Pipeline::Create(pipelineSpec);
		}

	}

	void Renderer3D::Shutdown()
	{
	}

	void Renderer3D::DrawModel(const glm::mat4& transform, const glm::vec3& cameraPos, MeshComponent& MeshComponent, int EntityID)
	{
		Ref<Shader> defaultShader = Library<Shader>::GetInstance().GetDefaultShader();
		defaultShader->Bind();
		if (MeshComponent.mMesh->bPlayAnim)
			defaultShader->SetBool("u_Animated", true);
		else
			defaultShader->SetBool("u_Animated", false);

		MeshComponent.mMesh->Draw(transform, cameraPos, Library<Shader>::GetInstance().GetDefaultShader(), EntityID);
	}

	void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		Ref<UniformBuffer> cameraUniform = Library<UniformBuffer>::GetInstance().GetCameraUniformBuffer();
		glm::mat4 ViewProjection = camera.GetProjection() * glm::inverse(transform);
		cameraUniform->SetData(&ViewProjection, sizeof(ViewProjection));
	}

	void Renderer3D::BeginScene(const EditorCamera& camera)
	{
		Ref<UniformBuffer> cameraUniform = Library<UniformBuffer>::GetInstance().GetCameraUniformBuffer();
		glm::mat4 ViewProjection = camera.GetViewProjection();
		cameraUniform->SetData(&ViewProjection, sizeof(ViewProjection));
	}

	void Renderer3D::EndScene()
	{
	}

}