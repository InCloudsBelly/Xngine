#include "Xpch.h"

#include "Runtime/Platform/OpenGL/OpenGLPostProcessing.h"

#include <glad/glad.h>

namespace X
{
	static uint32_t DoMSAA(const Ref<Framebuffer>& fb)
	{
		uint32_t width = fb->GetSpecification().Width;
		uint32_t height = fb->GetSpecification().Height;
		unsigned int intermediateFBO;
		glGenFramebuffers(1, &intermediateFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

		//create a color attachment texture

		unsigned int screenTexture;
		glGenTextures(1, &screenTexture);
		glBindTexture(GL_TEXTURE_2D, screenTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// we only need a color buffer

		//unsigned int tempTex;
		//glGenTextures(1, &tempTex);
		//glBindTexture(GL_TEXTURE_2D, tempTex);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tempTex, 0);	// we only need a color buffer

		unsigned int tempTex;
		glGenRenderbuffers(1, &tempTex);
		glBindRenderbuffer(GL_RENDERBUFFER, tempTex);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_R32I, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 1, GL_RENDERBUFFER, tempTex);


		fb->BindReadFramebuffer();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		//glDrawBuffer(intermediateFBO);
		//glReadBuffer(intermediateFBO);
		//glBindTexture(GL_TEXTURE_2D, screenTexture);


		return screenTexture;

	}

	uint32_t OpenGLPostProcessing::ExcuteAndReturnFinalTex(const Ref<Framebuffer>& fb)
	{
		switch (mType)
		{
		case X::PostProcessingType::None:
			return 0;
			break;
		case X::PostProcessingType::MSAA:
			return DoMSAA(fb);
			break;
		default:
			return 0;
			break;
		}
		return 0;
	}
}