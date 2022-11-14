#include "Xpch.h"

#include "Runtime/EcsFramework/System/Render/RenderSystem3D.h"
#include "Runtime/EcsFramework/System/Render/EnvironmentSystem.h"
#include "Runtime/EcsFramework/Component/ComponentGroup.h"
#include "Runtime/EcsFramework/Entity/Entity.h"

#include "Runtime/Renderer/Renderer3D.h"
#include "Runtime/Renderer/RenderCommand.h"
#include "Runtime/Renderer/Framebuffer.h"
#include "Runtime/Renderer/UniformBuffer.h"
#include "Runtime/Library/ShaderLibrary.h"
#include "Runtime/Library/UniformBufferLibrary.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"



namespace X
{
	namespace Utils
	{
		static std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview)
		{
			const auto inv = glm::inverse(projview);

			std::vector<glm::vec4> frustumCorners;
			for (unsigned int x = 0; x < 2; ++x)
			{
				for (unsigned int y = 0; y < 2; ++y)
				{
					for (unsigned int z = 0; z < 2; ++z)
					{
						const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
						frustumCorners.push_back(pt / pt.w);
					}
				}
			}

			return frustumCorners;
		}

		static glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane, float cameraFOV, float aspect, const glm::vec3& lightDir, const glm::mat4& view)
		{
			const auto proj = glm::perspective(
				glm::radians(cameraFOV), aspect, nearPlane,
				farPlane);
			const auto corners = getFrustumCornersWorldSpace(proj * view);

			glm::vec3 center = glm::vec3(0, 0, 0);
			for (const auto& v : corners)
			{
				center += glm::vec3(v);
			}
			center /= corners.size();

			const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::min();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::min();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::min();
			for (const auto& v : corners)
			{
				const auto trf = lightView * v;
				minX = std::min(minX, trf.x);
				maxX = std::max(maxX, trf.x);
				minY = std::min(minY, trf.y);
				maxY = std::max(maxY, trf.y);
				minZ = std::min(minZ, trf.z);
				maxZ = std::max(maxZ, trf.z);
			}

			// Tune this parameter according to the scene
			constexpr float zMult = 10.0f;
			if (minZ < 0)
			{
				minZ *= zMult;
			}
			else
			{
				minZ /= zMult;
			}
			if (maxZ < 0)
			{
				maxZ /= zMult;
			}
			else
			{
				maxZ *= zMult;
			}

			const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

			return lightProjection * lightView;
		}

		static std::vector<glm::mat4> getLightSpaceMatrices(float cameraNearPlane, float cameraFarPlane, float cameraFOV, float aspect, const glm::vec3& lightDir, const glm::mat4& projview, const std::vector<float>& shadowCascadeLevels)
		{
			std::vector<glm::mat4> ret;
			for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
			{
				if (i == 0)
				{
					ret.push_back(getLightSpaceMatrix(cameraNearPlane, shadowCascadeLevels[i], cameraFOV, aspect, lightDir, projview));
				}
				else if (i < shadowCascadeLevels.size())
				{
					ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i], cameraFOV, aspect, lightDir, projview));
				}
				else
				{
					ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], cameraFarPlane, cameraFOV, aspect, lightDir, projview));
				}
			}
			return ret;
		}
	}

	void RenderSystem3D::OnUpdateRuntime(Timestep ts)
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

		if (mainCamera)
		{
			Renderer3D::BeginScene(*mainCamera, cameraTransform);


			// Draw model
			{
				auto view = mLevel->mRegistry.view<TransformComponent, MeshComponent>();
				for (auto entity : view)
				{
					auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);

					Renderer3D::DrawModel(transform.GetTransform(), cameraPos, mesh, (int)entity);
				}
			}

			Renderer3D::EndScene();
		}
	}

	void RenderSystem3D::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer3D::BeginScene(camera);

		Ref<Shader> defaultShader = Library<Shader>::GetInstance().GetDefaultShader();
	
		if (ModeManager::bHdrUse)
			defaultShader->SetFloat("exposure", EnvironmentSystem::environmentSettings.exposure);
		else
			defaultShader->SetFloat("exposure", 1.0f);

		// directional light reset
		defaultShader->SetInt("cascadeCount", -2);
		//for (size_t i = 0; i < 4; ++i)
		//{
		//	defaultShader->SetFloat("cascadePlaneDistances[" + std::to_string(i) + "]", 1);
		//}
		defaultShader->SetFloat3("lightDir", glm::vec3(0.0f));


		//HBAO
		{
			//Gbuffer Pass
			{
				Ref<Framebuffer> gfb = Renderer3D::HBAOGbufferPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer;
				gfb->Bind();

				RenderCommand::SetViewport(0, 0, gfb->GetSpecification().Width, gfb->GetSpecification().Height);
				//RenderCommand::SetClearColor({ 0.4f, 0.4f, 0.4f, 1 });
				RenderCommand::Clear();

				RenderCommand::DepthTest(true);
				/*	RenderCommand::Cull(true);
					RenderCommand::CullFrontOrBack(false);*/

				Ref<Shader> gShader = Renderer3D::HBAOGbufferPipeline->GetSpecification().Shader;
				auto view = mLevel->mRegistry.view<TransformComponent, MeshComponent>();
				for (auto e : view)
				{
					Entity entity = { e, mLevel };
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& mesh = entity.GetComponent<MeshComponent>();


					gShader->Bind();

					//
					gShader->SetMat4("view", camera.GetViewMatrix());
					gShader->SetFloat("u_Near", camera.GetNearPlane());
					gShader->SetFloat("u_Far", camera.GetFarPlane());
					//

					if (mesh.mMesh->bPlayAnim)
						gShader->SetBool("u_Animated", true);
					else
						gShader->SetBool("u_Animated", false);

					mesh.mMesh->Draw(transform.GetTransform(), camera.GetPosition(), Renderer3D::HBAOGbufferPipeline, (int)e);
				}				
				gfb->Unbind();
			}

			//HBAO
			{
				Ref<Framebuffer> hbaofb = Renderer3D::HBAOPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer;
				Ref<Framebuffer> gfb = Renderer3D::HBAOGbufferPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer;
				X_ASSERT(hbaofb->GetSpecification().Height == gfb->GetSpecification().Height && hbaofb->GetSpecification().Width == gfb->GetSpecification().Width, "U forget resize framebuffer!");

				uint32_t colorAttachmentIndex = 0;

				Ref<Shader> hbaoShader = Renderer3D::HBAOPipeline->GetSpecification().Shader;

				hbaofb->Bind();
				RenderCommand::Clear();

				uint32_t width = hbaofb->GetSpecification().Width;
				uint32_t height = hbaofb->GetSpecification().Height;


				//Normal Tex
				gfb->BindColorTex2D(0, 2);
				//Depth Tex
				gfb->BindDepthTex2D(1);
				//Nosie Tex
				Renderer3D::TextureConfigNoise->Bind(2);

				glm::vec2 FocalLen;
				float fovRad = glm::radians(camera.GetFOV());
				FocalLen[0] = 1.0f / tanf(fovRad * 0.5f) * ((float)height / (float)width);
				FocalLen[1] = 1.0f / tanf(fovRad * 0.5f);

				hbaoShader->Bind();
				hbaoShader->SetInt("u_NormalTexture", 0);
				hbaoShader->SetInt("u_DepthTexture", 1);
				hbaoShader->SetInt("u_NoiseTexture", 2);

				hbaoShader->SetFloat("u_Near", camera.GetNearPlane());
				hbaoShader->SetFloat("u_Far", camera.GetFarPlane());
				hbaoShader->SetFloat("u_Fov", camera.GetFOV());
				hbaoShader->SetFloat2("u_FocalLen", FocalLen);
				hbaoShader->SetFloat("u_WindowWidth", width);
				hbaoShader->SetFloat("u_WindowHeight", height);

				Renderer3D::HBAOPipeline->DrawQuad();
				hbaofb->Unbind();
			}

			//HBAO Blur
			{
				Ref<Framebuffer> hbaoBlurfb = Renderer3D::HBAOBlurPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer;
				Ref<Framebuffer> hbaofb = Renderer3D::HBAOPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer;
				X_ASSERT(hbaoBlurfb->GetSpecification().Height == hbaofb->GetSpecification().Height && hbaoBlurfb->GetSpecification().Width == hbaofb->GetSpecification().Width, "U forget resize framebuffer!");

				uint32_t colorAttachmentIndex = 0;

				Ref<Shader> hbaoBlurShader = Renderer3D::HBAOBlurPipeline->GetSpecification().Shader;

				hbaoBlurfb->Bind();
				RenderCommand::Clear();

				uint32_t width = hbaoBlurfb->GetSpecification().Width;
				uint32_t height = hbaoBlurfb->GetSpecification().Height;

				//HBAO Tex
				hbaofb->BindColorTex2D(0, 0);

				hbaoBlurShader->Bind();
				hbaoBlurShader->SetInt("u_HBAOTexture", 0);

				Renderer3D::HBAOBlurPipeline->DrawQuad();
				hbaoBlurfb->Unbind();
			}

		}


		//Point Light
		{
			auto view = mLevel->mRegistry.view<TransformComponent, PointLightComponent>();
			int i = 0;
			for (auto entity : view)
			{
				auto [transform, light] = view.get<TransformComponent, PointLightComponent>(entity);

				glm::vec3 lightPos = transform.GetTranslation();
				float intensity = light.Intensity;
				glm::vec3 lightColor = light.LightColor;

				defaultShader->Bind();
				defaultShader->SetFloat3("lightPositions[" + std::to_string(i) + "]", lightPos);
				defaultShader->SetFloat3("lightColors[" + std::to_string(i) + "]", intensity * lightColor);

				i++;
			}
			if (i == 0)
			{
				for (size_t i = 0; i < 4; i++)
				{
					defaultShader->Bind();
					defaultShader->SetFloat3("lightColors[" + std::to_string(i) + "]", glm::vec3{ -1.0f });
				}
			}
		}

		// Directional light
		{
			auto view = mLevel->mRegistry.view<TransformComponent, DirectionalLightComponent>();

			defaultShader->Bind();
			defaultShader->SetInt("shadowMap", 8);
			Renderer3D::lightPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer->UnBindDepthTex3D(8);


			for (auto e : view)
			{
				Entity entity = { e, mLevel };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& directionalLight = entity.GetComponent<DirectionalLightComponent>();

				float cameraNearPlane = camera.GetNearPlane();
				float cameraFarPlane = camera.GetFarPlane();
				std::vector<float> shadowCascadeLevels{ cameraFarPlane / 50.0f, cameraFarPlane / 25.0f, cameraFarPlane / 10.0f, cameraFarPlane / 2.0f };


				glm::vec3 lightDir = glm::normalize(glm::eulerAngles(glm::quat(transform.Rotation)));

				const auto lightMatrices = Utils::getLightSpaceMatrices(cameraNearPlane, cameraFarPlane, camera.GetFOV(),
					camera.GetAspect(), lightDir, camera.GetViewMatrix(), shadowCascadeLevels);
				Ref<UniformBuffer> lightMatricesUBO = Library<UniformBuffer>::GetInstance().Get("LightMatricesUniform");
				for (size_t i = 0; i < lightMatrices.size(); i++)
				{
					lightMatricesUBO->SetData(&lightMatrices[i], sizeof(glm::mat4x4), i * sizeof(glm::mat4x4));
				}

				defaultShader->SetMat4("view", camera.GetViewMatrix());
				defaultShader->SetFloat3("lightDir", lightDir);
				defaultShader->SetFloat("farPlane", cameraFarPlane);
				defaultShader->SetInt("cascadeCount", shadowCascadeLevels.size());
				defaultShader->SetFloat("dirLightIntensity", directionalLight.Intensity);

				for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
				{
					defaultShader->SetFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
				}
				Renderer3D::lightPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer->BindDepthTex3D(8);
				//only one depth map now
				break;
			}
		}


		// Light Depth pass
		{
			Ref<Framebuffer> shadowfb = Renderer3D::lightPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer;
			shadowfb->Bind();
			RenderCommand::SetViewport(0, 0, shadowfb->GetSpecification().Width, shadowfb->GetSpecification().Height);

			RenderCommand::Clear();
			RenderCommand::CullFrontOrBack(true); // peter panning


			auto view = mLevel->mRegistry.view<TransformComponent, MeshComponent>();
			for (auto e : view)
			{
				Entity entity = { e, mLevel };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& mesh = entity.GetComponent<MeshComponent>();

				Ref<Shader> csmShader = Renderer3D::lightPipeline->GetSpecification().Shader;

				csmShader->Bind();
				if (mesh.mMesh->bPlayAnim)
					csmShader->SetBool("u_Animated", true);
				else
					csmShader->SetBool("u_Animated", false);

				mesh.mMesh->Draw(transform.GetTransform(), camera.GetPosition(), Renderer3D::lightPipeline, (int)e);
			}
			RenderCommand::CullFrontOrBack(false);
		}


		// Render pass
		{
			Ref<Framebuffer> geofb = Renderer3D::GeometryPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer;
			geofb->Bind();

			RenderCommand::SetViewport(0, 0, geofb->GetSpecification().Width, geofb->GetSpecification().Height);

			auto view = mLevel->mRegistry.view<TransformComponent, MeshComponent>();
			for (auto e : view)
			{
				Entity entity = { e, mLevel };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& mesh = entity.GetComponent<MeshComponent>();

				Ref<Shader> mainShader = Renderer3D::GeometryPipeline->GetSpecification().Shader;
				mainShader->Bind();
				if (mesh.mMesh->bPlayAnim)
					mainShader->SetBool("u_Animated", true);
				else
					mainShader->SetBool("u_Animated", false);

				if ((int)e == ConfigManager::selectedEntity)
				{
					RenderCommand::SetStencilFunc(StencilFunc::ALWAYS, 1, 0xFF);
					RenderCommand::StencilMask(0xFF);
					mesh.mMesh->Draw(transform.GetTransform(), camera.GetPosition(), Renderer3D::GeometryPipeline, (int)e);

					RenderCommand::SetStencilFunc(StencilFunc::NOTEQUAL, 1, 0xFF);
					RenderCommand::StencilMask(0x00);
					if (!mesh.mMesh->bPlayAnim)
						mesh.mMesh->Draw(transform.GetTransform(), camera.GetPosition(), Library<Shader>::GetInstance().Get("NormalOutline"), (int)e);
					else
						mesh.mMesh->Draw(transform.GetTransform(), camera.GetPosition(), Library<Shader>::GetInstance().Get("NormalOutline_anim"), (int)e);

					RenderCommand::StencilMask(0xFF);
					RenderCommand::SetStencilFunc(StencilFunc::ALWAYS, 0, 0xFF);
					RenderCommand::ClearStencil();
				}
				else
					mesh.mMesh->Draw(transform.GetTransform(), camera.GetPosition(), Renderer3D::GeometryPipeline, (int)e);
			}
		}

		//QuadPass
		{
			Ref<Framebuffer> quadFb = Renderer3D::HBAOQuadPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer;
			Ref<Framebuffer> showFb = Renderer3D::GeometryPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer;
			X_ASSERT(showFb->GetSpecification().Height == quadFb->GetSpecification().Height && showFb->GetSpecification().Width == quadFb->GetSpecification().Width, "U forget resize framebuffer!");
			//HBAO
			Ref<Framebuffer> hbaoBlurFb = Renderer3D::HBAOBlurPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer;

			uint32_t colorAttachmentIndex = 0;

			Ref<Shader> quadShader = Renderer3D::HBAOQuadPipeline->GetSpecification().Shader;

			quadFb->Bind();
			RenderCommand::Clear();

			uint32_t width = quadFb->GetSpecification().Width;
			uint32_t height = quadFb->GetSpecification().Height;


			if (showFb->GetSpecification().Samples > 1)
			{
				showFb->BindReadFramebuffer(colorAttachmentIndex);
				quadFb->BindDrawFramebuffer();
				RenderCommand::BlitFramebuffer(width,height);

				RenderCommand::UnbindReadBuffer();

				quadShader->Bind();
				quadFb->BindColorTex2D(0, 0);
				hbaoBlurFb->BindColorTex2D(1, 0);
				//Basic quad
				quadShader->SetInt("ComTexture", 0);
				//HBAO tes
				quadShader->SetInt("HBAOBlur", 1);
				Renderer3D::QuadPipeline->DrawQuad();
				RenderCommand::UnbindDrawBuffer();

			}
			else
			{
				quadShader->Bind();
				showFb->BindColorTex2D(0, 0);
				hbaoBlurFb->BindColorTex2D(1, 0);

				//Basic quad
				quadShader->SetInt("ComTexture", 0);
				//HBAO tes
				quadShader->SetInt("HBAOBlur", 1);
				Renderer3D::QuadPipeline->DrawQuad();
			}
			quadFb->Unbind();
		}
		Renderer3D::GeometryPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer->Bind();
		defaultShader->Bind();

		Renderer3D::EndScene();
	}
}