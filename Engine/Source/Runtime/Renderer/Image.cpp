//#include "Xpch.h"
//#include "Runtime/Platform/OpenGL/OpenGLImage.h"
//#include "Image.h"
//#include "Runtime/Renderer/RendererAPI.h"
//
//
//namespace X
//{
//
//	Ref<Image2D> Image2D::Create(ImageFormat format, uint32_t width, uint32_t height, Buffer buffer)
//	{
//		switch (RendererAPI::Current())
//		{
//		case RendererAPI::RendererAPIType::None:   return CreateRef<Image2D>();
//		case RendererAPI::RendererAPIType::OpenGL: return CreateRef<OpenGLImage2D>(format, width, height, buffer);
//		case RendererAPI::RendererAPIType::Vulkan: return nullptr;
//		case RendererAPI::RendererAPIType::DX11:   return nullptr;
//		}
//		X_CORE_ASSERT(false, "Unknown RendererAPI");
//		return Ref<Image2D>();
//	}
//
//	Ref<Image2D> Image2D::Create(ImageFormat format, uint32_t width, uint32_t height, const void* data)
//	{
//		switch (RendererAPI::Current())
//		{
//		case RendererAPI::RendererAPIType::None:   return Ref<Image2D>();
//		case RendererAPI::RendererAPIType::OpenGL: return CreateRef<OpenGLImage2D>(format, width, height, data);
//		case RendererAPI::RendererAPIType::Vulkan: return nullptr;
//		case RendererAPI::RendererAPIType::DX11:   return nullptr;
//		}
//		X_CORE_ASSERT(false, "Unknown RendererAPI");
//		return Ref<Image2D>();
//	}
//
//	Ref<Image2D> Image2D::Create(ImageSpecification specification, const void* data)
//	{
//		switch (RendererAPI::Current())
//		{
//		case RendererAPI::RendererAPIType::None:   return Ref<Image2D>();
//		case RendererAPI::RendererAPIType::OpenGL: return CreateRef<OpenGLImage2D>(specification);
//		case RendererAPI::RendererAPIType::Vulkan: return nullptr;
//		case RendererAPI::RendererAPIType::DX11:   return nullptr;
//		}
//		X_CORE_ASSERT(false, "Unknown RendererAPI");
//		return Ref<Image2D>();
//	}
//
//}
