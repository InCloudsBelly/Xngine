#pragma once

#include "Runtime/Renderer/Texture.h"
#include "Runtime/Renderer/Shader.h"
#include "Runtime/Library/ShaderLibrary.h"

#include <unordered_map>

namespace X
{
    struct EnumClassHash
    {
        template <typename T>
        std::size_t operator()(T t) const
        {
            return static_cast<std::size_t>(t);
        }
    };

    enum class TextureType
    {
        Albedo = 0,
        Normal,
        Metalness,
        Roughness,
        AmbientOcclusion,
        Specular,
        Height,
        Emission,
    };

    struct MaterialTexture
    {
        Ref<Texture2D> texture2d = nullptr;
        TextureType type;
        std::string path;
    };

    class Material
    {
    public:
        Material() = default;
        Material(Ref<Shader> shader) : mShader(shader) {}
    public:
        void SetShader(Ref<Shader> shader) { mShader = shader; }
        [[nodiscard]] Ref<Shader> GetShader() { return mShader; }

        void AddTexture(TextureType type, Ref<Texture2D> texture)
        {
            X_CORE_ASSERT(mTexMap.find(type) == mTexMap.end());
            mTexMap[type] = texture;
        }

        [[nodiscard]] Ref<Texture2D> GetTexture(TextureType type) { return mTexMap[type]; }

    public:
        std::vector<MaterialTexture> mTextures;
    private:
        Ref<Shader> mShader;
        std::unordered_map<TextureType, Ref<Texture2D>, EnumClassHash> mTexMap;
    };
}