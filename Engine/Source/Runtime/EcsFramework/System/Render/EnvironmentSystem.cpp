#include "Xpch.h"

#include "Runtime/EcsFramework/System/Render/EnvironmentSystem.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"
#include "Runtime/EcsFramework/Component/ComponentGroup.h"
#include "Runtime/EcsFramework/Entity/Entity.h"
#include "Runtime/Renderer/RenderCommand.h"
#include "Runtime/Library/UniformBufferLibrary.h"

#include "Runtime/Library/ShaderLibrary.h"

#include "Runtime/Renderer/Renderer3D.h"
#include "Runtime/Renderer/Framebuffer.h"

#include "Editor/Panels/SceneSettingsPanel.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "Runtime/Platform/OpenGL/OpenGLEnvironment.h"

namespace X
{
	static uint32_t id = 0;
	static uint32_t oldId = 0;

	EnvironmentHdrSettings EnvironmentSystem::environmentSettings;

	void EnvironmentSystem::OnUpdateRuntime(Timestep ts)
	{
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		glm::vec3 cameraPos;
		{
			auto view = mLevel->mRegistry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					cameraPos = transform.GetTranslation();
					break;
				}
			}
		}

		switch (ModeManager::mSceneMode)
		{
		case SceneMode::None:
			break;
		case SceneMode::EnvironmentHdr:
			if (mainCamera)
				DrawEnvironmentHdr(glm::inverse(cameraTransform), mainCamera->GetProjection());
			break;
		case SceneMode::SkyBox:
			if (mainCamera)
				DrawSkyBox(glm::inverse(cameraTransform), mainCamera->GetProjection());
			break;
		default:
			break;
		}
	}

	void EnvironmentSystem::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		switch (ModeManager::mSceneMode)
		{
		case SceneMode::None:
			break;
		case SceneMode::EnvironmentHdr:
			DrawEnvironmentHdr(camera.GetViewMatrix(), camera.GetProjection());
			break;
		case SceneMode::SkyBox:
			DrawSkyBox(camera.GetViewMatrix(), camera.GetProjection());
			break;
		default:
			break;
		}
	}

	void EnvironmentSystem::DrawSkyBox(const glm::mat4& ViewMatrix, const glm::mat4& ProjectMatrix)
	{

		Ref<UniformBuffer> cameraUniform = Library<UniformBuffer>::GetInstance().GetCameraUniformBuffer();
		glm::mat4 ViewProjection = ProjectMatrix * glm::mat4(glm::mat3(ViewMatrix));
		cameraUniform->SetData(&ViewProjection, sizeof(ViewProjection));

		RenderCommand::Cull(0);

		RenderCommand::DepthFunc(DepthComp::LEQUAL);
	
		oldId = id;
		Ref<CubeMapTexture> skyboxTex = Library<CubeMapTexture>::GetInstance().Get("SkyBoxTexture");
		id = skyboxTex->GetRendererID();

		if (id != oldId)
		{
			int framebufferOld = 0;
			framebufferOld = RenderCommand::GetDrawFrameBuffer();

			ComputeIrradiance(skyboxTex);

			ComputePrefilterMap(skyboxTex);

			RenderCommand::BindFrameBuffer(framebufferOld);
		}
		skyboxTex->Bind(0);

		Library<Shader>::GetInstance().GetSkyBoxShader()->Bind();

		Library<Shader>::GetInstance().GetSkyBoxShader()->SetInt("SkyBox", 0);
		Library<Mesh>::GetInstance().Get("Box")->Draw();

		RenderCommand::DepthFunc(DepthComp::LESS);
	}

	void EnvironmentSystem::DrawEnvironmentHdr(const glm::mat4& ViewMatrix, const glm::mat4& ProjectMatrix)
	{
		if (!ModeManager::bHdrUse)
		{
			return;
		}
		Ref<UniformBuffer> cameraUniform = Library<UniformBuffer>::GetInstance().GetCameraUniformBuffer();
		glm::mat4 ViewProjection = ProjectMatrix * glm::mat4(glm::mat3(ViewMatrix));
		cameraUniform->SetData(&ViewProjection, sizeof(ViewProjection));


		oldId = id;
		Ref<Texture2D> hdrTex = Library<Texture2D>::GetInstance().Get("DefaultHdr");
		id = hdrTex->GetRendererID();
		
		Ref<CubeMapTexture> envCubemap = Library<CubeMapTexture>::GetInstance().Get("EnvironmentHdr");

		if (id!= oldId)
		{

			int framebufferOld = 0;
			framebufferOld = RenderCommand::GetDrawFrameBuffer();

			HDR2CubeTex(hdrTex, envCubemap);

			ComputeIrradiance(envCubemap);

			ComputePrefilterMap(envCubemap);

			RenderCommand::BindFrameBuffer(framebufferOld);
		}
		
		envCubemap->Bind(0);
		RenderCommand::SetViewport(0, 0, ConfigManager::mViewportSize.x, ConfigManager::mViewportSize.y);

		RenderCommand::DepthFunc(DepthComp::LEQUAL);

		Ref<Shader> backgroundShader = Library<Shader>::GetInstance().Get("IBL_background");
		backgroundShader->Bind();
		backgroundShader->SetInt("environmentMap", 0);
		backgroundShader->SetFloat("SkyBoxLod", environmentSettings.SkyBoxLod);
		backgroundShader->SetFloat("exposure", environmentSettings.exposure);

		Library<Mesh>::GetInstance().Get("Box")->Draw();
		RenderCommand::DepthFunc(DepthComp::LESS);
	}
}