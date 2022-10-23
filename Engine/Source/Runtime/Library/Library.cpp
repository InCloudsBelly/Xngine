#include "Xpch.h"

#include "Runtime/Library/Library.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Renderer/Model.h"
#include "Runtime/Renderer/Texture.h"
#include "UniformBufferLibrary.h"

namespace X
{
    template<typename LibType>
    Library<LibType>::Library()
    {
    }

    template<>
    Library<Model>::Library()
    {
        Add("Box", CreateRef<Model>("Assets/Models/Box.obj"));
    }

    template<>
    Library<CubeMapTexture>::Library()
    {
        std::vector<std::string> Paths{
            "Assets/Textures/Skybox/right.jpg",
            "Assets/Textures/Skybox/left.jpg",
            "Assets/Textures/Skybox/top.jpg",
            "Assets/Textures/Skybox/bottom.jpg",
            "Assets/Textures/Skybox/front.jpg",
            "Assets/Textures/Skybox/back.jpg",
        };
        Add("SkyBoxTexture", CubeMapTexture::Create(Paths));
        Add("EnvironmentHdr", CubeMapTexture::Create(512, 512));
        Add("EnvironmentIrradiance", CubeMapTexture::Create(32, 32));
        Add("EnvironmentPrefilter", CubeMapTexture::Create(128, 128));
    }
}