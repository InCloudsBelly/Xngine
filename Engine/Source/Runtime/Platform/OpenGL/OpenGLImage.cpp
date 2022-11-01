//
//#include "Xpch.h"
//#include "OpenGLImage.h"
//
//#include "Runtime/Renderer/Renderer.h"
//
//
//namespace X {
//
//	OpenGLImage2D::OpenGLImage2D(ImageSpecification specification)
//		: m_Format(specification.Format), m_Width(specification.Width), m_Height(specification.Height)
//	{
//	}
//
//	OpenGLImage2D::OpenGLImage2D(ImageSpecification specification, Buffer buffer)
//		: m_Format(specification.Format), m_Width(specification.Width), m_Height(specification.Height), m_ImageData(buffer)
//	{
//	}
//
//	OpenGLImage2D::OpenGLImage2D(ImageSpecification specification, const void* data)
//		: m_Format(specification.Format), m_Width(specification.Width), m_Height(specification.Height)
//	{
//		if (data)
//		{
//			m_ImageData = Buffer::Copy(data, Utils::GetImageMemorySize(m_Format, m_Width, m_Height));
//		}
//	}
//
//	OpenGLImage2D::OpenGLImage2D(ImageFormat format, uint32_t width, uint32_t height, Buffer buffer)
//		: m_Format(format), m_Width(width), m_Height(height), m_ImageData(buffer)
//	{
//	}
//
//	OpenGLImage2D::OpenGLImage2D(ImageFormat format, uint32_t width, uint32_t height, const void* data)
//		: m_Format(format), m_Width(width), m_Height(height)
//	{
//		// TODO: Local storage should be optional
//		if (data)
//		{
//			m_ImageData = Buffer::Copy(data, Utils::GetImageMemorySize(format, width, height));
//		}
//	}
//
//	OpenGLImage2D::~OpenGLImage2D()
//	{
//		// Should this be submitted?
//		m_ImageData.Release();
//		// HazelRenderer::Submit([=]() {});
//		{
//			if (m_RendererID)
//			{
//				glDeleteTextures(1, &m_RendererID);
//			}
//			// Release();
//		}
//	}
//
//	void OpenGLImage2D::Invalidate()
//	{
//		if (m_RendererID) { Release(); }
//
//		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
//		// glBindTexture(GL_TEXTURE_2D, m_RendererID);
//
//		GLenum internalFormat = Utils::OpenGLImageInternalFormat(m_Format);
//
//		uint32_t mipCount = Utils::CalculateMipCount(m_Width, m_Height);
//		glTextureStorage2D(m_RendererID, mipCount, internalFormat, m_Width, m_Height);
//
//		if (m_ImageData)
//		{
//			GLenum format = Utils::OpenGLImageFormat(m_Format);
//			GLenum dataType = Utils::OpenGLFormatDataType(m_Format);
//			// glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, format, dataType, m_ImageData.Data);
//			// glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
//			// glGenerateTextureMipmap(m_RendererID);
//		}
//
//		// Sampler
//		glCreateSamplers(1, &m_SamplerRendererID);
//		glSamplerParameteri(m_SamplerRendererID, GL_TEXTURE_MIN_FILTER, m_ImageData ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
//		glSamplerParameteri(m_SamplerRendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glSamplerParameteri(m_SamplerRendererID, GL_TEXTURE_WRAP_R, GL_REPEAT);
//		glSamplerParameteri(m_SamplerRendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glSamplerParameteri(m_SamplerRendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	}
//
//	void OpenGLImage2D::InvalidateOld(bool srgb, TextureWrap wrap, bool isHDR)
//	{
//		if (srgb)
//		{
//			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
//			int levels = Utils::CalculateMipCount(m_Width, m_Height);
//			glTextureStorage2D(m_RendererID, levels, GL_SRGB8, m_Width, m_Height);
//			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
//			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, m_ImageData.Data);
//			glGenerateTextureMipmap(m_RendererID);
//		}
//		else
//		{
//			glGenTextures(1, &m_RendererID);
//			glBindTexture(GL_TEXTURE_2D, m_RendererID);
//
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//			GLenum localWrap = (wrap == TextureWrap::Clamp) ? GL_CLAMP_TO_EDGE : GL_REPEAT;
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)localWrap);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)localWrap);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, (GLint)localWrap);
//
//			GLenum internalFormat = Utils::OpenGLImageInternalFormat(m_Format);
//			GLenum format = srgb ? GL_SRGB8 : (isHDR ? GL_RGB : Utils::OpenGLImageFormat(m_Format)); // HDR = GL_RGB for no
//			GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
//			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, format, type, m_ImageData.Data);
//			glGenerateMipmap(GL_TEXTURE_2D);
//			glBindTexture(GL_TEXTURE_2D, 0);
//		}
//
//		// stbi_image_free(m_ImageData.Data);
//	}
//
//	void OpenGLImage2D::Release()
//	{
//		m_ImageData.Release();
//		if (m_RendererID)
//		{
//			glDeleteTextures(1, &m_RendererID);
//			m_RendererID = 0;
//		}
//	}
//
//}
