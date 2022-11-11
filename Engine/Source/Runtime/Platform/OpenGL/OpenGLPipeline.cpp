

#include "OpenGLPipeline.h"

#include "Runtime/Renderer/Renderer.h"

#include <glad/glad.h>


namespace X
{

	Ref<VertexArray>  OpenGLPipeline::mQuadVAO = nullptr;

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Float2:   return GL_FLOAT;
		case ShaderDataType::Float3:   return GL_FLOAT;
		case ShaderDataType::Float4:   return GL_FLOAT;
		case ShaderDataType::Mat3:     return GL_FLOAT;
		case ShaderDataType::Mat4:     return GL_FLOAT;
		case ShaderDataType::Int:      return GL_INT;
		case ShaderDataType::Int2:     return GL_INT;
		case ShaderDataType::Int3:     return GL_INT;
		case ShaderDataType::Int4:     return GL_INT;
		case ShaderDataType::Bool:     return GL_BOOL;
		}
		return 0;
	}


	static void GenerateLayout(const BufferLayout & layout)
	{
		uint32_t attribIndex = 0;
		for (const auto& element : layout)
		{

			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			{
				glEnableVertexAttribArray(attribIndex);
				glVertexAttribPointer(attribIndex,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)element.Offset);
				attribIndex++;
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(attribIndex);
				glVertexAttribIPointer(attribIndex,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					layout.GetStride(),
					(const void*)element.Offset);
				attribIndex++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				uint8_t count = element.GetComponentCount();
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(attribIndex);
					glVertexAttribPointer(attribIndex,
						count,
						ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)(element.Offset + sizeof(float) * count * i));
					glVertexAttribDivisor(attribIndex, 1);
					attribIndex++;
				}
				break;
			}
			default:
				X_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}
		}
	}

	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& spec)
		: mSpecification(spec)
	{
		Invalidate();
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
		GLuint rendererID = mVertexArrayRendererID;

		glDeleteVertexArrays(1, &rendererID);
	}

	void OpenGLPipeline::Invalidate()
	{
		X_CORE_ASSERT(mSpecification.StaticLayout.GetElements().size(), "StaticLayout is empty!");
		X_CORE_ASSERT(mSpecification.AnimationLayout.GetElements().size(), "AnimationLayout is empty!");

		auto& vertexArrayRendererID = this->mVertexArrayRendererID;

		if (vertexArrayRendererID)
			glDeleteVertexArrays(1, &vertexArrayRendererID);

		glGenVertexArrays(1, &vertexArrayRendererID);
		
		glBindVertexArray(vertexArrayRendererID);
		glBindVertexArray(0);

	}

	void OpenGLPipeline::BindStatic()
	{
		glBindVertexArray(this->mVertexArrayRendererID);
		const auto& layout = this->mSpecification.StaticLayout;
		GenerateLayout(layout);
	}

	void OpenGLPipeline::BindAnimation()
	{
		glBindVertexArray(this->mVertexArrayRendererID);
		const auto& layout = this->mSpecification.AnimationLayout;
		GenerateLayout(layout);
	}

	void OpenGLPipeline::Unbind()
	{
		glBindVertexArray(0);
	}

	void OpenGLPipeline::InitQuad()
	{
		mQuadVAO = VertexArray::Create();
		float screenQuadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			-1.0f,  1.0f, /* 0.0f, 1.0f,*/
			-1.0f, -1.0f, /* 0.0f, 0.0f,*/
			 1.0f, -1.0f, /* 1.0f, 0.0f,*/
			 1.0f,  1.0f, /* 1.0f, 1.0f*/
		};
		uint32_t screenQuadIndices[] = {
			0, 1, 2, 0, 2, 3
		};
		Ref<IndexBuffer> ScreenQuadIBO = IndexBuffer::Create(screenQuadIndices, 6);
		Ref<VertexBuffer> ScreenQuadVBO = VertexBuffer::Create(screenQuadVertices, sizeof(screenQuadVertices));
		ScreenQuadVBO->SetLayout({
			{ ShaderDataType::Float2,	"a_Position" },
			});
		mQuadVAO->AddVertexBuffer(ScreenQuadVBO);
		mQuadVAO->SetIndexBuffer(ScreenQuadIBO);
	}

	void OpenGLPipeline::DrawQuad()
	{
		if (!bQuadInited)
		{
			InitQuad();
			bQuadInited = true;
		}

		RenderCommand::DrawIndexed(mQuadVAO);
	}
}
