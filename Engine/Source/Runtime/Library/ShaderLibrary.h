#pragma once

#include "Runtime/Renderer/Shader.h"
#include "Runtime/Library/Library.h"


namespace X
{
	template<>
	class Library<Shader> : public LibraryBase<Library, Shader>
	{
	public:
		Library();
		[[nodiscard]] Ref<Shader> GetDefaultShader();
		[[nodiscard]] Ref<Shader> GetSkyBoxShader();

	};
}