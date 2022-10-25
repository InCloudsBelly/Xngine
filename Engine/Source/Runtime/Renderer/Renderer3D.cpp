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
	Ref<Framebuffer> Renderer3D::lightFBO = nullptr;

	void Renderer3D::Init()
	{
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::DEPTH32F_TEX3D };
		fbSpec.Width = ConfigManager::mViewportSize.x;
		fbSpec.Height = ConfigManager::mViewportSize.y;

		lightFBO = Framebuffer::Create(fbSpec);
	}

	void Renderer3D::Shutdown()
	{
	}

	void Renderer3D::DrawModel(const glm::mat4& transform, const glm::vec3& cameraPos, MeshComponent& MeshComponent, int EntityID)
	{
		if (ModeManager::bHdrUse)
		{
			if (MeshComponent.mMesh->bPlayAnim)
				MeshComponent.mMesh->Draw(transform, cameraPos, Library<Shader>::GetInstance().Get("IBL_pbr_anim"), EntityID);
			else
				MeshComponent.mMesh->Draw(transform, cameraPos, Library<Shader>::GetInstance().Get("IBL_pbr_static"), EntityID);
		}
		else
		{
			if (MeshComponent.mMesh->bPlayAnim)
				MeshComponent.mMesh->Draw(transform, cameraPos, Library<Shader>::GetInstance().Get("BasePBR_anim"), EntityID);
			else
				MeshComponent.mMesh->Draw(transform, cameraPos, Library<Shader>::GetInstance().Get("BasePBR"), EntityID);
		}
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