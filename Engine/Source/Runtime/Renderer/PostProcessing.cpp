#include "Xpch.h"

#include "Runtime/Renderer/PostProcessing.h"
#include "Runtime/Renderer/RenderCommand.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"


namespace X
{
    Ref<VertexArray>  PostProcessing::mScreenQuadVAO = nullptr;
    Ref<VertexBuffer> PostProcessing::mScreenQuadVBO = nullptr;
    Ref<IndexBuffer>  PostProcessing::mScreenQuadIBO = nullptr;
    Ref<Framebuffer>  PostProcessing::mFramebuffer = nullptr;
    Ref<Texture2D>    PostProcessing::mIntermediateScreenTex = nullptr;


    PostProcessing::PostProcessing(const PostProcessingType& type)
        : mType{ type }
    {
    }

    void PostProcessing::Init()
    {
        mScreenQuadVAO = VertexArray::Create();
        float screenQuadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };
        uint32_t screenQuadIndices[] = {
            0, 1, 2, 0, 2, 3
        };
        mScreenQuadIBO = IndexBuffer::Create(screenQuadIndices, 6);
        mScreenQuadVBO = VertexBuffer::Create(screenQuadVertices, sizeof(screenQuadVertices));
        mScreenQuadVBO->SetLayout({
            { ShaderDataType::Float2,	"a_Position" },
            { ShaderDataType::Float2,	"a_TexCoord" },
            });
        mScreenQuadVAO->AddVertexBuffer(mScreenQuadVBO);
        mScreenQuadVAO->SetIndexBuffer(mScreenQuadIBO);

        FramebufferSpecification fbSpec;
        fbSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
        fbSpec.Width = ConfigManager::mViewportSize.x;
        fbSpec.Height = ConfigManager::mViewportSize.y;
        mFramebuffer = Framebuffer::Create(fbSpec);
    }

    std::string PostProcessing::PostTypeToString(PostProcessingType type)
    {
        switch (type)
        {
        case X::PostProcessingType::None:
            return "";
            break;
        case X::PostProcessingType::MSAA:
            return "MSAA";
            break;
        case X::PostProcessingType::Outline:
            return "Outline";
            break;
        case X::PostProcessingType::Cartoon:
            return "Cartoon";
            break;
        case X::PostProcessingType::GrayScale:
            return "GrayScale";
            break;
        case X::PostProcessingType::GaussianBlur:
            return "GaussianBlur";
            break;
        case X::PostProcessingType::ComputeTest:
            return "ComputeTest";
            break;
        default:
            break;
        }
        return std::string();
    }


    void PostProcessing::DoPostProcessing()
    {
        RenderCommand::DrawIndexed(mScreenQuadVAO);
    }

}