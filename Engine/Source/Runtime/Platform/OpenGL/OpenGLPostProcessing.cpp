#include "Xpch.h"

#include "Runtime/Platform/OpenGL/OpenGLPostProcessing.h"
#include "Runtime/Platform/OpenGL/OpenGLTexture.h"
#include "Runtime/Library/ShaderLibrary.h"

#include <glad/glad.h>

namespace X
{
    uint32_t OpenGLPostProcessing::DoMSAA(const Ref<Framebuffer>& fb, const uint32_t& colorIndex)
    {
        uint32_t width = fb->GetSpecification().Width;
        uint32_t height = fb->GetSpecification().Height;
       
        mFramebuffer->Bind();

        if (!mAlreadyProcessed)
        {
            fb->BindReadFramebuffer(colorIndex);
            mAlreadyProcessed = true;
        }
        else
            mFramebuffer->BindReadFramebuffer();

        mFramebuffer->BindDrawFramebuffer();

        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        return mFramebuffer->GetColorAttachmentRendererID();
    }

    uint32_t OpenGLPostProcessing::DoPostWithShader(const Ref<Framebuffer>& fb, const Ref<Shader>& shader, const uint32_t& colorIndex)
    {
        uint32_t width = fb->GetSpecification().Width;
        uint32_t height = fb->GetSpecification().Height;
        mFramebuffer->Bind();

        // copy the framebuffer to the intermediate screen texture
        if (!mAlreadyProcessed)
        {
            fb->BindReadFramebuffer(colorIndex);
            mAlreadyProcessed = true;
        }
        else
            mFramebuffer->BindReadFramebuffer();

        mIntermediateScreenTex->Bind();
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

        mFramebuffer->BindDrawFramebuffer();

        shader->Bind();
        shader->SetInt("screenTexture", 0);
        shader->SetInt("depthMap", 0);
        DoPostProcessing();

        return mFramebuffer->GetColorAttachmentRendererID();
    }

    uint32_t OpenGLPostProcessing::DoPostWithComputeShader(const Ref<Framebuffer>& fb, const Ref<Shader>& shader,const uint32_t& colorIndex)
    {
        uint32_t width = fb->GetSpecification().Width;
        uint32_t height = fb->GetSpecification().Height;
        mFramebuffer->Bind();

        // copy the framebuffer to the intermediate screen texture
        if (!mAlreadyProcessed)
        {
            fb->BindReadFramebuffer(colorIndex);
            mAlreadyProcessed = true;
        }
        else
            mFramebuffer->BindReadFramebuffer();

        mIntermediateScreenTex->Bind();

        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

        shader->Bind();
        shader->setImage(0, mIntermediateScreenTex);
        
        std::vector<int> LocalGroupSize, GlobalGroupSize;
        shader->GetLocalGroupSize(LocalGroupSize);
        GlobalGroupSize.push_back((width + LocalGroupSize[0] - 1) / LocalGroupSize[0]);
        GlobalGroupSize.push_back((height + LocalGroupSize[1] - 1) / LocalGroupSize[1]);
        GlobalGroupSize.push_back(1);

        shader->ComputeDispatch(GlobalGroupSize[0], GlobalGroupSize[1], GlobalGroupSize[2]);
        shader->setBarrier(BarrierType::ShaderImageAccess);
        return mIntermediateScreenTex->GetRendererID();
    }




    uint32_t OpenGLPostProcessing::ExcuteAndReturnFinalTex(const Ref<Framebuffer>& fb, const uint32_t& colorIndex)
    {
        uint32_t re = 0;
        switch (mType)
        {
        case PostProcessingType::None:
            return 0;
            break;
        case PostProcessingType::MSAA:
            re = DoMSAA(fb, colorIndex);
            break;
        case PostProcessingType::Outline:
            re = DoPostWithShader(fb, Library<Shader>::GetInstance().Get("Post_Outline") ,colorIndex);
            break;
        case PostProcessingType::Cartoon:
            re = DoPostWithShader(fb, Library<Shader>::GetInstance().Get("Post_Cartoon"),colorIndex);
            break;
        case PostProcessingType::GrayScale:
            re = DoPostWithShader(fb, Library<Shader>::GetInstance().Get("Post_GrayScale"),colorIndex);
            break;
        case PostProcessingType::GaussianBlur:
            re = DoPostWithShader(fb, Library<Shader>::GetInstance().Get("Post_GaussianBlur"),colorIndex);
            break;
        case PostProcessingType::ComputeTest:
            re = DoPostWithComputeShader(fb, Library<Shader>::GetInstance().Get("ComputeTest"),colorIndex);
            break;
        default:
            return 0;
            break;
        }
        // it seems imgui need it ??
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        return re;
    }
}