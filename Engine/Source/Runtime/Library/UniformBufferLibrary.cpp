#include "Xpch.h"

#include "Runtime/Library/UniformBufferLibrary.h"

namespace X
{
    Library<UniformBuffer>::Library()
    {
        //Base PBR
        Ref<UniformBuffer> CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0);
        Add("CameraUniform", CameraUniformBuffer);

        Ref<UniformBuffer> LightMatricesUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4x4) * 16, 1);
        Add("LightMatricesUniform", LightMatricesUniformBuffer);

        //other

    }

    Ref<UniformBuffer> Library<UniformBuffer>::GetCameraUniformBuffer()
    {
        return mLibrary["CameraUniform"];
    }
}