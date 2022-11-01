#pragma once

#include "Runtime/Core/Base/Base.h"
#include "Runtime/Renderer/Image.h"
#include <string>
#include <filesystem>


namespace X
{
    enum class TextureFormat
    {
        None = 0,
        RGB = 1,
        RGBA = 2,
        Float16 = 3
    };

    enum class TextureType
    {
        None = 0,
        Texture2D,
        TextureCube
    };

    class Texture
    {
    public:
        virtual ~Texture() = default;

        [[nodiscard]] virtual uint32_t GetWidth() const = 0;
        [[nodiscard]] virtual uint32_t GetHeight() const = 0;
        [[nodiscard]] virtual uint32_t GetRendererID() const = 0;

        virtual void SetData(void* data, uint32_t size) {};

        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual void UnBind() const = 0;

        virtual bool IsLoaded() const { return false; };

        virtual bool operator==(const Texture& other) const = 0;
    };

    enum class FilterType
    {
        None = 0,
        Linear,
    };
    enum class WrapType
    {
        None = 0,
        Repeat,
        ClampToEdge,
        ClampToBorder
    };

    enum class DataFormat
    {
        None =0,
        RGBA,
        RGBA8,
        RGB,
        Alpha,

    };

    class Texture2D : public Texture
    {
    public:
        static Ref<Texture2D> Create(uint32_t width, uint32_t height,
            DataFormat internal = DataFormat::RGBA8,
            DataFormat external = DataFormat::RGBA8,
            DataFormat data = DataFormat::RGBA,
            FilterType min = FilterType::Linear,
            FilterType mag = FilterType::Linear,
            WrapType wraps = WrapType::Repeat,
            WrapType wrapt = WrapType::Repeat
        );
        static Ref<Texture2D> Create(const std::filesystem::path& path);
        static Ref<Texture2D> Create(const std::string& path);

        virtual void setBindUnit(unsigned int unit) = 0;
        virtual unsigned int GetUnit() = 0;

        virtual unsigned int GetInternalFormat() = 0;
       
    };



    class Texture3D : public Texture
    {
    public:
        static Ref<Texture3D> Create(uint32_t width, uint32_t height);
    };

    enum class FaceTarget : uint32_t
    {
        Right = 0,
        Left = 1,
        Top = 2,
        Bottom = 3,
        Front = 4,
        Back = 5,
    };

    class CubeMapTexture : public Texture
    {
    public:
        virtual void SetFace(FaceTarget faceIndex, const std::string& path) = 0;
        virtual void GenerateMipmap() = 0;
        static Ref<CubeMapTexture> Create(std::vector<std::string>& paths);
        static Ref<CubeMapTexture> Create();
        static Ref<CubeMapTexture> Create(uint32_t width, uint32_t height);

        virtual void Generate() = 0;

        [[nodiscard]] virtual std::vector<std::string> GetPaths() = 0;
    };
}


