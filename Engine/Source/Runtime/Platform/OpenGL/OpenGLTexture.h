#pragma once

#include "Runtime/Renderer/Texture.h"

#include <glad/glad.h>

namespace X
{
    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(uint32_t width, uint32_t height,
            DataFormat internal, DataFormat external, DataFormat data,
            FilterType min, FilterType mag, 
            WrapType wraps, WrapType wrapt);

        OpenGLTexture2D(const std::filesystem::path& path);
        virtual ~OpenGLTexture2D();

        virtual uint32_t GetWidth() const override { return mWidth; };
        virtual uint32_t GetHeight() const override { return mHeight; };
        virtual uint32_t GetRendererID() const override { return mRendererID; }

        virtual void SetData(void* data, uint32_t size) override;

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void UnBind() const override;

		virtual bool IsLoaded() const override { return mIsLoaded; }
        
        virtual bool operator==(const Texture& other) const override
        {
            return mRendererID == ((OpenGLTexture2D&)other).mRendererID;
        }

       virtual void setBindUnit(unsigned int unit) override
        {
            mBindingUnit = unit;
        }
       virtual unsigned int GetUnit() override
        {
            return mBindingUnit;
        }

       virtual GLenum GetInternalFormat() override
        {
            return mInternalFormat;
        }

    private:
        GLint mBindingUnit = -1;
        std::filesystem::path mPath;
		bool mIsLoaded = false;
        uint32_t mWidth, mHeight;
        uint32_t mRendererID;
        GLenum mInternalFormat, mExternalFormat, mDataFormat;
    };


    class OpenGLTexture3D : public Texture3D
    {
    public:
        OpenGLTexture3D(uint32_t width, uint32_t height);
        OpenGLTexture3D(uint32_t rendererID, uint32_t width, uint32_t height);
        virtual ~OpenGLTexture3D();

        virtual uint32_t GetWidth() const override { return mWidth; };
        virtual uint32_t GetHeight() const override { return mHeight; };
        virtual uint32_t GetRendererID() const override { return mRendererID; }

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void UnBind() const override;

        virtual bool operator==(const Texture& other) const override
        {
            return mRendererID == ((OpenGLTexture3D&)other).mRendererID;
        }
    private:
        uint32_t mWidth, mHeight;
        uint32_t mRendererID;
        //GLenum mInternalFormat, mDataFormat;
    };

    class OpenGLCubeMapTexture :public CubeMapTexture
    {
    public:
        OpenGLCubeMapTexture();
        OpenGLCubeMapTexture(uint32_t width, uint32_t height);
        OpenGLCubeMapTexture(std::vector<std::string>& paths);
        virtual ~OpenGLCubeMapTexture();

        virtual uint32_t GetWidth() const override { return mWidth; };
        virtual uint32_t GetHeight() const override { return mHeight; };
        virtual uint32_t GetRendererID() const override { return mRendererID; }

        virtual void SetFace(FaceTarget faceIndex, const std::string& path) override;
        virtual void GenerateMipmap() override;

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void UnBind() const override;

        virtual void Generate() override;
        [[nodiscard]] virtual std::vector<std::string> GetPaths() override { return mPaths; }

        virtual bool operator==(const Texture& other) const override
        {
            return mRendererID == ((OpenGLCubeMapTexture&)other).mRendererID;
        }
    private:
        uint32_t mRendererID;
        uint32_t mWidth, mHeight;
        std::vector<std::string> mPaths;
    };
}

