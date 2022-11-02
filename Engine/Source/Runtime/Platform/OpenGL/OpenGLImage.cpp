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
//		: mFormat(specification.Format), mWidth(specification.Width), mHeight(specification.Height)
//	{
//	}
//
//	OpenGLImage2D::OpenGLImage2D(ImageSpecification specification, Buffer buffer)
//		: mFormat(specification.Format), mWidth(specification.Width), mHeight(specification.Height), mImageData(buffer)
//	{
//	}
//
//	OpenGLImage2D::OpenGLImage2D(ImageSpecification specification, const void* data)
//		: mFormat(specification.Format), mWidth(specification.Width), mHeight(specification.Height)
//	{
//		if (data)
//		{
//			mImageData = Buffer::Copy(data, Utils::GetImageMemorySize(mFormat, mWidth, mHeight));
//		}
//	}
//
//	OpenGLImage2D::OpenGLImage2D(ImageFormat format, uint32_t width, uint32_t height, Buffer buffer)
//		: mFormat(format), mWidth(width), mHeight(height), mImageData(buffer)
//	{
//	}
//
//	OpenGLImage2D::OpenGLImage2D(ImageFormat format, uint32_t width, uint32_t height, const void* data)
//		: mFormat(format), mWidth(width), mHeight(height)
//	{
//		// TODO: Local storage should be optional
//		if (data)
//		{
//			mImageData = Buffer::Copy(data, Utils::GetImageMemorySize(format, width, height));
//		}
//	}
//
//	OpenGLImage2D::~OpenGLImage2D()
//	{
//		// Should this be submitted?
//		mImageData.Release();
//		// HazelRenderer::Submit([=]() {});
//		{
//			if (mRendererID)
//			{
//				glDeleteTextures(1, &mRendererID);
//			}
//			// Release();
//		}
//	}
//
//	void OpenGLImage2D::Invalidate()
//	{
//		if (mRendererID) { Release(); }
//
//		glCreateTextures(GL_TEXTURE_2D, 1, &mRendererID);
//		// glBindTexture(GL_TEXTURE_2D, mRendererID);
//
//		GLenum internalFormat = Utils::OpenGLImageInternalFormat(mFormat);
//
//		uint32_t mipCount = Utils::CalculateMipCount(mWidth, mHeight);
//		glTextureStorage2D(mRendererID, mipCount, internalFormat, mWidth, mHeight);
//
//		if (mImageData)
//		{
//			GLenum format = Utils::OpenGLImageFormat(mFormat);
//			GLenum dataType = Utils::OpenGLFormatDataType(mFormat);
//			// glTextureSubImage2D(mRendererID, 0, 0, 0, mWidth, mHeight, format, dataType, mImageData.Data);
//			// glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
//			// glGenerateTextureMipmap(mRendererID);
//		}
//
//		// Sampler
//		glCreateSamplers(1, &mSamplerRendererID);
//		glSamplerParameteri(mSamplerRendererID, GL_TEXTURE_MIN_FILTER, mImageData ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
//		glSamplerParameteri(mSamplerRendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glSamplerParameteri(mSamplerRendererID, GL_TEXTURE_WRAP_R, GL_REPEAT);
//		glSamplerParameteri(mSamplerRendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glSamplerParameteri(mSamplerRendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	}
//
//	void OpenGLImage2D::InvalidateOld(bool srgb, TextureWrap wrap, bool isHDR)
//	{
//		if (srgb)
//		{
//			glCreateTextures(GL_TEXTURE_2D, 1, &mRendererID);
//			int levels = Utils::CalculateMipCount(mWidth, mHeight);
//			glTextureStorage2D(mRendererID, levels, GL_SRGB8, mWidth, mHeight);
//			glTextureParameteri(mRendererID, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
//			glTextureParameteri(mRendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//			glTextureSubImage2D(mRendererID, 0, 0, 0, mWidth, mHeight, GL_RGB, GL_UNSIGNED_BYTE, mImageData.Data);
//			glGenerateTextureMipmap(mRendererID);
//		}
//		else
//		{
//			glGenTextures(1, &mRendererID);
//			glBindTexture(GL_TEXTURE_2D, mRendererID);
//
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//			GLenum localWrap = (wrap == TextureWrap::Clamp) ? GL_CLAMP_TO_EDGE : GL_REPEAT;
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)localWrap);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)localWrap);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, (GLint)localWrap);
//
//			GLenum internalFormat = Utils::OpenGLImageInternalFormat(mFormat);
//			GLenum format = srgb ? GL_SRGB8 : (isHDR ? GL_RGB : Utils::OpenGLImageFormat(mFormat)); // HDR = GL_RGB for no
//			GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
//			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, format, type, mImageData.Data);
//			glGenerateMipmap(GL_TEXTURE_2D);
//			glBindTexture(GL_TEXTURE_2D, 0);
//		}
//
//		// stbi_image_free(mImageData.Data);
//	}
//
//	void OpenGLImage2D::Release()
//	{
//		mImageData.Release();
//		if (mRendererID)
//		{
//			glDeleteTextures(1, &mRendererID);
//			mRendererID = 0;
//		}
//	}
//
//}
