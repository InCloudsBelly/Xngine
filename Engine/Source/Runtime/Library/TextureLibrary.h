#pragma once

#include "Runtime/Renderer/Texture.h"
#include "Runtime/Library/Library.h"


namespace X
{
	template<>
	class Library<Texture2D> : public LibraryBase<Library, Texture2D>
	{
	public:
		Library();
		[[nodiscard]] Ref<Texture2D> GetDefaultTexture();
		[[nodiscard]] Ref<Texture2D> GetWhiteTexture();

		[[nodiscard]] Ref<Texture2D> GetOrCreateTexture(std::string path);
	};
}