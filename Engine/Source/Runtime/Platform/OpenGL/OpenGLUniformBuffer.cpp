#include "Xpch.h"

#include "Runtime/Platform/OpenGL/OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace X
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		glCreateBuffers(1, &mRendererID);
		glNamedBufferData(mRendererID, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, mRendererID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &mRendererID);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		glNamedBufferSubData(mRendererID, offset, size, data);
	}
}