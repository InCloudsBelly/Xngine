#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include "Runtime/Library/ShaderLibrary.h"
#include "Runtime/Renderer/Renderer3D.h"
#include "Runtime/Renderer/RenderCommand.h"

namespace X
{

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};


	void HDR2CubeTex(Ref<Texture2D> hdrTex, Ref<CubeMapTexture> envCubemap)
	{
		Ref<Shader> equirectangularToCubemapShader = Library<Shader>::GetInstance().Get("IBL_equirectangularToCubemap");
		equirectangularToCubemapShader->Bind();
		equirectangularToCubemapShader->SetInt("equirectangularMap", 0);
		equirectangularToCubemapShader->SetMat4("projection", captureProjection);

		//Temp
		unsigned int captureFBO;
		unsigned int captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		hdrTex->Bind();
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		RenderCommand::SetViewport(0, 0, envCubemap->GetWidth(), envCubemap->GetHeight());
		for (unsigned int i = 0; i < 6; ++i)
		{
			equirectangularToCubemapShader->SetMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap->GetRendererID(), 0);
			RenderCommand::Clear();

			Library<Mesh>::GetInstance().Get("Box")->Draw();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void ComputeIrradiance(Ref<CubeMapTexture> envCubemap)
	{

		envCubemap->Bind(0);
		envCubemap->GenerateMipmap();

		// irradiance map
		Ref<CubeMapTexture> irradianceMap = Library<CubeMapTexture>::GetInstance().Get("EnvironmentIrradiance");
		Ref<Shader> irradianceShader = Library<Shader>::GetInstance().Get("IBL_irradiance");



		//Temp
		// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
		// --------------------------------------------------------------------------------
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap->GetRendererID());
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		unsigned int captureFBO;
		unsigned int captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

		// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
		// -----------------------------------------------------------------------------
		irradianceShader->Bind();
		irradianceShader->SetInt("environmentMap", 0);
		irradianceShader->SetMat4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap->GetRendererID());

		glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		for (unsigned int i = 0; i < 6; ++i)
		{
			irradianceShader->SetMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap->GetRendererID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Library<Mesh>::GetInstance().Get("Box")->Draw();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Library<CubeMapTexture>::GetInstance().Set("EnvironmentIrradiance", irradianceMap);
	}

	void ComputePrefilterMap(Ref<CubeMapTexture> envCubemap)
	{
		// prefilter map
		Ref<CubeMapTexture> prefilterMap = Library<CubeMapTexture>::GetInstance().Get("EnvironmentPrefilter");
		Ref<Shader> prefilterShader = Library<Shader>::GetInstance().Get("IBL_prefilter");
		prefilterMap->Bind();
		prefilterMap->GenerateMipmap();
		prefilterShader->Bind();
		prefilterShader->SetInt("environmentMap", 0);
		prefilterShader->SetMat4("projection", captureProjection);
		envCubemap->Bind(0);

		// Temp
		unsigned int captureFBO;
		unsigned int captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
			unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			prefilterShader->SetFloat("roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				prefilterShader->SetMat4("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap->GetRendererID(), mip);

				RenderCommand::Clear();
				Library<Mesh>::GetInstance().Get("Box")->Draw();
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// End temp

		Library<CubeMapTexture>::GetInstance().Set("EnvironmentPrefilter", prefilterMap);
	}
}