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

namespace X
{
	Ref<Pipeline> Renderer3D::GbufferPipeline = nullptr;
	Ref<Framebuffer> Renderer3D::lightFBO = nullptr;
	Ref<Pipeline> Renderer3D::lightPipeline = nullptr;
	Ref<Pipeline> Renderer3D::GeometryPipeline = nullptr;

	void Renderer3D::Init()
	{
		//Gbuffer pass
		{
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA8,FramebufferTextureFormat::RGBA8 ,FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::DEPTH24STENCIL8 };
			fbSpec.Width = 1280;
			fbSpec.Height = 720;
			fbSpec.Samples = 1;
			Ref<Framebuffer> GbufferFB = Framebuffer::Create(fbSpec);

			RenderPassSpecification GbufferPassSpec;
			GbufferPassSpec.DebugName = "GbufferPass";
			GbufferPassSpec.TargetFramebuffer = GbufferFB;

			PipelineSpecification pipelineSpec;
			pipelineSpec.DebugName = GbufferPassSpec.DebugName;
			pipelineSpec.Shader = Library<Shader>::GetInstance().Get("Gbuffer");
			pipelineSpec.RenderPass = RenderPass::Create(GbufferPassSpec);
			//pipelineSpec.RenderPass->AddPostProcessing(PostProcessingType::MSAA);

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


			GbufferPipeline = Pipeline::Create(pipelineSpec);
		}


		//Directional light shadow pass
		{
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = { FramebufferTextureFormat::DEPTH32F_TEX3D };
			// light depth texture uses high resolution
			fbSpec.Width = 4096;
			fbSpec.Height = 4096;

			lightFBO = Framebuffer::Create(fbSpec);

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
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::RGBA8,  FramebufferTextureFormat::DEPTH24STENCIL8 };
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