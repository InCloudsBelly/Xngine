//#pragma once
//
//#include "Runtime/Core/Base/Base.h"
//#include "Runtime/Renderer/Texture.h"
//
//namespace X
//{
//	enum class AttachmentType
//	{
//		None = 0,
//		Texture = 1,
//		Renderbuffer = 2,
//	};
//
//	enum class AttachmentFormat
//	{
//		None = 0,
//		Color = 1,
//		Depth = 2, // GL_DEPTH_COMPONENT
//		Stencil = 3, // GL_STENCIL
//		Depth_24 = 4, // GL_DEPTH_COMPONENT24
//		DepthStencil = 5,
//		Depth_24_Stencil_8 = 6, // GL_DEPTH24_STENCIL8
//		RGBA = 7,
//		RGBA16F = 8,
//		RGBA8 = 9,
//		RGB = 10, // ::ImageFormat
//		RGBA32F = 11, // ::ImageFormat
//		RG32F = 12, // ::ImageFormat
//		SRGB = 13, // ::ImageFormat
//		DEPTH32F = 14, // ::ImageFormat
//		RED_INTEGER = 15, // For storing EntityIDs
//	};
//
//	class Attachment : public Texture
//	{
//	public:
//		Attachment();
//		Attachment(unsigned int width, unsigned int height, AttachmentType attachmentType, AttachmentFormat attachmentFormat, unsigned int orderID, uint32_t framebufferID);
//		virtual ~Attachment();
//
//		inline uint32_t GetID() const { return m_ID; };
//		inline uint32_t GetRendererID() const { return m_ID; };
//		inline virtual uint32_t GetWidth() const override { return m_Width; };
//		inline virtual uint32_t GetHeight() const override { return m_Height; };
//
//		virtual void Bind(uint32_t slot) const = 0;
//		virtual void Unbind() = 0;
//
//		// virtual methods from ::HazelTexture
//		virtual ImageFormat GetFormat() const override;
//		virtual uint64_t GetHash() const override { X_CORE_ERROR("Attachment::GetHash - method not implemented!"); return 0; }
//		virtual TextureType GetType() const override { X_CORE_ERROR("Attachment::GetType - method not implemented!"); return TextureType(); }
//		virtual bool operator==(const Texture& other) const override { X_CORE_ERROR("Attachment::operator== - method not implemented!"); return false; }
//		virtual void SetData(void* data, uint32_t size) override { X_CORE_ERROR("Attachment::SetData - method not implemented!"); }
//		virtual uint32_t GetMipLevelCount() const override { X_CORE_ERROR("Attachment::GetMipLevelCount - method not implemented!"); return 0; }
//		virtual std::pair<uint32_t, uint32_t> GetMipSize(uint32_t mip) const override;
//
//	protected:
//		unsigned int m_ID;
//		unsigned int m_Width;
//		unsigned int m_Height;
//		AttachmentType m_AttachmentType;
//		AttachmentFormat m_AttachmentFormat;
//		unsigned int m_OrderID; // used for color texture attachments GL_COLOR_ATTACHMENT0 + m_OrderID
//
//		uint32_t m_FramebufferID;
//
//	};
//}