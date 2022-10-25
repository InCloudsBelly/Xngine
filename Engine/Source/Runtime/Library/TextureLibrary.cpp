#include "Xpch.h"

#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Library/TextureLibrary.h"

namespace X
{
    Library<Texture2D>::Library()
    {
        Add("DefaultTexture", Texture2D::Create(std::string("Assets/Textures/DefaultTexture.png")));
        Add("DefaultNormal", Texture2D::Create(std::string("Assets/Textures/DefaultNormal.png")));
        Add("DefaultMetallicRoughness", Texture2D::Create(std::string("Assets/Textures/black.png")));
        Add("DefaultHdr", Texture2D::Create(std::string("Assets/Textures/Hdr/Default.hdr")));
        Add("BRDF_LUT", Texture2D::Create(std::string("Assets/Textures/BRDF_LUT.tga")));

        Ref<Texture2D> whiteTexture = Texture2D::Create(1, 1);
        uint32_t whiteTextureData = 0xffffffff;
        whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
        Add("WhiteTexture", whiteTexture);


        Ref<Texture2D> blackTexture = Texture2D::Create(1, 1);
        uint32_t blackTextureData = 0x00000000;
        blackTexture->SetData(&blackTextureData, sizeof(uint32_t));
        Add("BlackTexture", blackTexture);
    }
    
    Ref<Texture2D> Library<Texture2D>::GetDefaultTexture()
    {
        return mLibrary["DefaultTexture"];
    }

    Ref<Texture2D> Library<Texture2D>::GetWhiteTexture()
    {
        return mLibrary["WhiteTexture"];
    }
}
