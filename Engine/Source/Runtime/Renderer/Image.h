///**
// * @package  (Hazel to Morava)
// * @author  Yan Chernikov (TheCherno)
// * @licence Apache License 2.0
// */
//
//#pragma once
//
//#include "Runtime/Core/Base/Base.h"
//#include "Runtime/Core/Buffer.h"
//
//
//namespace X
//{
//
//	enum class ImageFormat
//	{
//		None = 0,
//		RGB,
//		RGBA,
//		RGBA8,
//		RGBA16F,
//		RGBA32F,
//		RG32F,
//
//		RED_INTEGER,
//
//		SRGB,
//
//		DEPTH32F,
//		DEPTH24STENCIL8,
//
//		// Defaults
//		Depth = DEPTH24STENCIL8
//	};
//
//	enum class ImageUsage
//	{
//		None = 0,
//		Texture,
//		Attachment,
//		Storage
//	};
//
//	enum class TextureWrap
//	{
//		None = 0,
//		Clamp,
//		Repeat
//	};
//
//	enum class TextureFilter
//	{
//		None = 0,
//		Linear,
//		Nearest
//	};
//
//	struct TextureProperties
//	{
//		TextureWrap SamplerWrap = TextureWrap::Repeat;
//		TextureFilter SamplerFilter = TextureFilter::Linear;
//		bool GenerateMips = true;
//		bool SRGB = false;
//		bool Storage = false;
//
//		std::string DebugName;
//	};
//
//	struct ImageSpecification
//	{
//		ImageFormat Format = ImageFormat::RGBA;
//		ImageUsage Usage = ImageUsage::Texture;
//		uint32_t Width = 1;
//		uint32_t Height = 1;
//		uint32_t Mips = 1;
//		uint32_t Layers = 1;
//		bool Deinterleaved = false;
//
//		std::string DebugName;
//	};
//
//	class Image
//	{
//	public:
//		virtual ~Image() {}
//
//		virtual void Invalidate() = 0;
//		virtual void Release() = 0;
//
//		virtual uint32_t GetWidth() const = 0;
//		virtual uint32_t GetHeight() const = 0;
//
//		virtual ImageFormat GetFormat() const = 0;
//		virtual float GetAspectRatio() const = 0;
//
//		virtual Buffer GetBuffer() const = 0;
//		virtual Buffer& GetBuffer() = 0;
//
//		virtual uint64_t GetHash() const = 0;
//
//		// TODO: usage (eg. shader read)
//	};
//
//	class Image2D : public Image
//	{
//	public:
//		static Ref<Image2D> Create(ImageFormat format, uint32_t width, uint32_t height, Buffer buffer);
//		static Ref<Image2D> Create(ImageFormat format, uint32_t width, uint32_t height, const void* data = nullptr);
//		static Ref<Image2D> Create(ImageSpecification specification, const void* data = nullptr);
//	};
//
//	namespace Utils {
//
//		inline uint32_t GetImageFormatBPP(ImageFormat format) // BPP - bytes per pixel
//		{
//			switch (format)
//			{
//			case ImageFormat::RGB:
//			case ImageFormat::SRGB:    return 3;
//			case ImageFormat::RGBA:    return 4;
//			case ImageFormat::RGBA8:   return 4;
//			case ImageFormat::RGBA16F: return 2 * 4;
//			case ImageFormat::RGBA32F: return 4 * 4;
//			}
//			X_CORE_ASSERT(false);
//			return 0;
//		}
//
//		inline uint32_t CalculateMipCount(uint32_t width, uint32_t height)
//		{
//			return (uint32_t)std::floor(std::log2(std::min(width, height))) + 1;
//		}
//
//		inline uint32_t GetImageMemorySize(ImageFormat format, uint32_t width, uint32_t height)
//		{
//			return width * height * GetImageFormatBPP(format);
//		}
//
//		inline bool IsDepthFormat(ImageFormat format)
//		{
//			if (format == ImageFormat::DEPTH24STENCIL8 || format == ImageFormat::DEPTH32F)
//			{
//				return true;
//			}
//
//			return false;
//		}
//
//	}
//
//}
