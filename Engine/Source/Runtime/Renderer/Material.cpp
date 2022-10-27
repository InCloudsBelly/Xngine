#include "Xpch.h"

#include "Runtime/Renderer/Material.h"

namespace X
{
	void Material::Initialize()
	{
		unsigned char data[4];

		for (size_t i = 0; i < 3; i++)
		{
			data[i] = (unsigned char)(metallic * 255.0f);
		}
		data[3] = (unsigned char)255.0f;
		metallicRGBA->SetData(data, sizeof(unsigned char) * 4);

		for (size_t i = 0; i < 3; i++)
		{
			data[i] = (unsigned char)(roughness * 255.0f);
		}
		data[3] = (unsigned char)255.0f;
		roughnessRGBA->SetData(data, sizeof(unsigned char) * 4);
	}


	void Material::LoadTextures()
	{
		if (!mAlbedoMapPath.empty())
			mAlbedoMap = Texture2D::Create(mAlbedoMapPath);
		if(!mNormalMapPath.empty())
			mNormalMap = Texture2D::Create(mNormalMapPath);
		if (!mMetallicMapPath.empty())
			mMetallicMap = Texture2D::Create(mMetallicMapPath);
		if (!mRoughnessMapPath.empty())
			mRoughnessMap = Texture2D::Create(mRoughnessMapPath);
		if (!mAoMapPath.empty())
			mAoMap = Texture2D::Create(mAoMapPath);
	}
}