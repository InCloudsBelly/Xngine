//
//
//#define _CRT_SECURE_NO_WARNINGS
//#pragma once
//
//
//#include "Runtime/Renderer/Image.h"
//#include "Runtime/Renderer/Texture.h"
////#include "Runtime/Renderer/RendererTypes.h"
//
//#include <glad/glad.h>
//
//
//namespace X
//{
//
//	class OpenGLImage2D : public Image2D
//	{
//	public:
//		OpenGLImage2D(ImageSpecification specification);
//		OpenGLImage2D(ImageSpecification specification, Buffer buffer);
//		OpenGLImage2D(ImageSpecification specification, const void* data);
//
//		OpenGLImage2D(ImageFormat format, uint32_t width, uint32_t height, Buffer buffer);
//		OpenGLImage2D(ImageFormat format, uint32_t width, uint32_t height, const void* data = nullptr);
//		virtual ~OpenGLImage2D();
//
//		virtual void Invalidate() override;
//		void InvalidateOld(bool srgb, TextureWrap wrap, bool isHDR); // Temporary method
//		virtual void Release() override;
//
//		virtual ImageFormat GetFormat() const override { return m_Format; }
//		virtual uint32_t GetWidth() const override { return m_Width; }
//		virtual uint32_t GetHeight() const override { return m_Height; }
//
//		virtual float GetAspectRatio() const override { return (float)m_Specification.Width / (float)m_Specification.Height; }
//
//		void RT_Invalidate() {}
//
//		virtual Buffer GetBuffer() const override { return m_ImageData; }
//		virtual Buffer& GetBuffer() override { return m_ImageData; }
//
//		virtual uint64_t GetHash() const override { return (uint64_t)m_RendererID; }
//
//		uint32_t& GetRendererID() { return m_RendererID; }
//		uint32_t GetRendererID() const { return m_RendererID; }
//
//		uint32_t& GetSamplerRendererID() { return m_SamplerRendererID; }
//		uint32_t GetSamplerRendererID() const { return m_SamplerRendererID; }
//
//	private:
//		ImageSpecification m_Specification;
//
//		uint32_t m_RendererID = 0;
//		uint32_t m_SamplerRendererID = 0;
//		uint32_t m_Width, m_Height;
//		ImageFormat m_Format;
//
//		Buffer m_ImageData;
//	};
//
//	namespace Utils {
//
//		inline GLenum OpenGLImageFormat(ImageFormat format)
//		{
//			switch (format)
//			{
//			case ImageFormat::RGB:     return GL_RGB;
//			case ImageFormat::SRGB:    return GL_RGB;
//			case ImageFormat::RGBA:
//			case ImageFormat::RGBA16F:
//			case ImageFormat::RGBA32F: return GL_RGBA;
//			}
//			X_CORE_ASSERT(false, "Unknown image format");
//			return 0;
//		}
//
//		inline GLenum OpenGLImageInternalFormat(ImageFormat format)
//		{
//			switch (format)
//			{
//			case ImageFormat::RGB:             return GL_RGB8;
//			case ImageFormat::SRGB:            return GL_SRGB8;
//			case ImageFormat::RGBA:            return GL_RGBA8;
//			case ImageFormat::RGBA16F:         return GL_RGBA16F;
//			case ImageFormat::RGBA32F:         return GL_RGBA32F;
//			case ImageFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
//			case ImageFormat::DEPTH32F:        return GL_DEPTH_COMPONENT32F;
//			}
//			X_CORE_ASSERT(false, "Unknown image format");
//			return 0;
//		}
//
//		inline GLenum OpenGLFormatDataType(ImageFormat format)
//		{
//			switch (format)
//			{
//			case ImageFormat::RGB:
//			case ImageFormat::SRGB:
//			case ImageFormat::RGBA:    return GL_UNSIGNED_BYTE;
//			case ImageFormat::RGBA16F:
//			case ImageFormat::RGBA32F: return GL_FLOAT;
//			}
//			X_CORE_ASSERT(false, "Unknown image format");
//			return 0;
//		}
//
//		inline GLenum OpenGLSamplerWrap(TextureWrap wrap)
//		{
//			switch (wrap)
//			{
//			case TextureWrap::Clamp:   return GL_CLAMP_TO_EDGE;
//			case TextureWrap::Repeat:  return GL_REPEAT;
//			}
//			X_CORE_ASSERT(false, "Unknown wrap mode");
//			return 0;
//		}
//
//		/****
//		// Note: should always be called with mipmap = false for magnification filtering
//		inline GLenum OpenGLSamplerFilter(TextureFilter filter, bool mipmap)
//		{
//			switch (filter)
//			{
//				case TextureFilter::Linear:   return mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
//				case TextureFilter::Nearest:  return mipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
//			}
//			_CORE_ASSERT(false, "Unknown filter");
//			return 0;
//		}
//		****/
//
//	}
//
//}
