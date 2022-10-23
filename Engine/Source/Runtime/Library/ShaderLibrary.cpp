#include "Xpch.h"

#include "Runtime/Library/ShaderLibrary.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"

namespace X
{
    Library<Shader>::Library()
    {
        Add("BasePBR", Shader::Create(AssetManager::GetFullPath("Shaders/BasePBR.glsl")));
        Add("SkyBox", Shader::Create(AssetManager::GetFullPath("Shaders/SkyBox.glsl")));


        Add("IBL_background", Shader::CreateNative(AssetManager::GetFullPath("Shaders/IBL/background.glsl")));
        Add("IBL_brdf", Shader::CreateNative(AssetManager::GetFullPath("Shaders/IBL/brdf.glsl")));
        Add("IBL_equirectangularToCubemap", Shader::CreateNative(AssetManager::GetFullPath("Shaders/IBL/equirectangularToCubemap.glsl")));
        Add("IBL_irradiance", Shader::CreateNative(AssetManager::GetFullPath("Shaders/IBL/irradiance.glsl")));
        Add("IBL_pbr_static", Shader::CreateNative(AssetManager::GetFullPath("Shaders/IBL/pbr_static.glsl")));
        Add("IBL_pbr_anim", Shader::CreateNative(AssetManager::GetFullPath("Shaders/IBL/pbr_anim.glsl")));
        Add("IBL_prefilter", Shader::CreateNative(AssetManager::GetFullPath("Shaders/IBL/prefilter.glsl")));
    }

    Ref<Shader> Library<Shader>::GetDefaultShader()
    {
        return mLibrary["BasePBR"];
    }

    Ref<Shader> Library<Shader>::GetSkyBoxShader()
    {
        return mLibrary["SkyBox"];
    }
}
