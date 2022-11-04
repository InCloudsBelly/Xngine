#include "Xpch.h"
#include "OpenGLRenderPass.h"
#include "OpenGLPostProcessing.h"

namespace X
{
    void OpenGLRenderPass::AddPostProcessing(PostProcessingType type)
    {
        mPostProcessings.emplace_back(CreateScope<OpenGLPostProcessing>(type)); 
    }

    uint32_t OpenGLRenderPass::ExcuteAndReturnFinalTex(const uint32_t& colorIndex)
    {
        PostProcessing::mIntermediateScreenTex = Texture2D::Create(
            mSpecification.TargetFramebuffer->GetSpecification().Width,
            mSpecification.TargetFramebuffer->GetSpecification().Height
        );
        PostProcessing::ResetPostProcessing();

        uint32_t final = 0;
        for (auto& effect : mPostProcessings)
        {
            final = effect->ExcuteAndReturnFinalTex(mSpecification.TargetFramebuffer, colorIndex);
        }

        return final;
    }
}