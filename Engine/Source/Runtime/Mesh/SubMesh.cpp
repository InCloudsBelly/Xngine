#include "Xpch.h"

#include "Runtime/Mesh/SubMesh.h"
#include "Runtime/Renderer/RenderCommand.h"
#include "Runtime/Library/TextureLibrary.h"
#include "Runtime/Mesh/Mesh.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"

namespace X
{
	SubMesh::SubMesh(const std::vector<StaticVertex>& vertices, const std::vector<uint32_t> indices)
		: mStaticVertices(vertices), mIndices(indices)
	{
		mVertexArray = VertexArray::Create();

		mVB = VertexBuffer::Create(sizeof(StaticVertex) * vertices.size());
		mVB->SetLayout({
					{ ShaderDataType::Float3, "a_Pos"},
					{ ShaderDataType::Float3, "a_Normal"},
					{ ShaderDataType::Float2, "a_TexCoord"},
					{ ShaderDataType::Float3, "a_Tangent"},
					{ ShaderDataType::Float3, "a_Bitangent"},
					{ ShaderDataType::Int,	  "a_EntityID"},
			});

		mVertexArray->AddVertexBuffer(mVB);

		mIB = IndexBuffer::Create(indices.size());

		mVertexArray->SetIndexBuffer(mIB);
	}

	SubMesh::SubMesh(const std::vector<StaticVertex>& vertices, const std::vector<uint32_t> indices, const std::vector<MaterialTexture>& textures, uint32_t materialIndex)
		: mStaticVertices(vertices), mIndices(indices), mTextures(textures), mMaterialIndex(materialIndex)
	{
		mVertexArray = VertexArray::Create();

		mVB = VertexBuffer::Create(sizeof(StaticVertex) * vertices.size());
		mVB->SetLayout({
					{ ShaderDataType::Float3, "a_Pos"},
					{ ShaderDataType::Float3, "a_Normal"},
					{ ShaderDataType::Float2, "a_TexCoord"},
					{ ShaderDataType::Float3, "a_Tangent"},
					{ ShaderDataType::Float3, "a_Bitangent"},
					{ ShaderDataType::Int,	  "a_EntityID"},
			});

		mVertexArray->AddVertexBuffer(mVB);

		mIB = IndexBuffer::Create(indices.size());

		mVertexArray->SetIndexBuffer(mIB);
	}

	SubMesh::SubMesh(const std::vector<SkinnedVertex>& vertices, const std::vector<uint32_t> indices)
		: mSkinnedVertices(vertices), mIndices(indices)
	{
		mVertexArray = VertexArray::Create();

		mVB = VertexBuffer::Create(sizeof(SkinnedVertex) * vertices.size());
		mVB->SetLayout({
					{ ShaderDataType::Float3, "a_Pos"},
					{ ShaderDataType::Float3, "a_Normal"},
					{ ShaderDataType::Float2, "a_TexCoord"},
					{ ShaderDataType::Float3, "a_Tangent"},
					{ ShaderDataType::Float3, "a_Bitangent"},
					{ ShaderDataType::Int,	  "a_EntityID"},
					{ ShaderDataType::Int4,   "a_BoneIDs"},
					{ ShaderDataType::Float4, "a_Weights"},
			});

		mVertexArray->AddVertexBuffer(mVB);

		mIB = IndexBuffer::Create(indices.size());

		mVertexArray->SetIndexBuffer(mIB);
	}

	SubMesh::SubMesh(const std::vector<SkinnedVertex>& vertices, const std::vector<uint32_t> indices, const std::vector<MaterialTexture>& textures, uint32_t materialIndex)
		: mSkinnedVertices(vertices), mIndices(indices), mTextures(textures), mMaterialIndex(materialIndex)
	{
		mVertexArray = VertexArray::Create();

		mVB = VertexBuffer::Create(sizeof(SkinnedVertex) * vertices.size());
		mVB->SetLayout({
					{ ShaderDataType::Float3, "a_Pos"},
					{ ShaderDataType::Float3, "a_Normal"},
					{ ShaderDataType::Float2, "a_TexCoord"},
					{ ShaderDataType::Float3, "a_Tangent"},
					{ ShaderDataType::Float3, "a_Bitangent"},
					{ ShaderDataType::Int,	  "a_EntityID"},
					{ ShaderDataType::Int4,   "a_BoneIDs"},
					{ ShaderDataType::Float4, "a_Weights"},
			});
		mVertexArray->AddVertexBuffer(mVB);
		mIB = IndexBuffer::Create(indices.size());
		mVertexArray->SetIndexBuffer(mIB);
	}

	void SubMesh::Draw(const glm::mat4& transform, const glm::vec3& cameraPos, const Ref<Shader>& shader, int entityID, Mesh* model)
	{
		SetupMesh(entityID);

		shader->Bind();

		if (model->bPlayAnim)
		{
			if (!model->bStopAnim)
				model->mAnimator.UpdateAnimation(0.01f * model->mAnimPlaySpeed);
			auto transforms = model->mAnimator.GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i)
				shader->SetMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
		}


		shader->SetMat4("model", transform);
		shader->SetFloat3("camPos", cameraPos);

		if (ModeManager::bHdrUse)
		{
			Library<CubeMapTexture>::GetInstance().Get("EnvironmentIrradiance")->Bind(0);
			Library<CubeMapTexture>::GetInstance().Get("EnvironmentPrefilter")->Bind(1);
			Library<Texture2D>::GetInstance().Get("BRDF_LUT")->Bind(2);
		}
		else
		{
			Library<CubeMapTexture>::GetInstance().Get("BlackCubeMap")->Bind(0);
			Library<CubeMapTexture>::GetInstance().Get("BlackCubeMap")->Bind(1);
			Library<Texture2D>::GetInstance().Get("BlackTexture")->Bind(2);
		}

		if (model->mMaterial[mMaterialIndex]->bUseAlbedoMap)
			model->mMaterial[mMaterialIndex]->mAlbedoMap->Bind(3);
		else
			model->mMaterial[mMaterialIndex]->albedoRGBA->Bind(3);

		if (model->mMaterial[mMaterialIndex]->bUseNormalMap)
			model->mMaterial[mMaterialIndex]->mNormalMap->Bind(4);
		else
			Library<Texture2D>::GetInstance().GetWhiteTexture()->Bind(4);

		if (model->mMaterial[mMaterialIndex]->bUseMetallicMap)
			model->mMaterial[mMaterialIndex]->mMetallicMap->Bind(5);
		else
			model->mMaterial[mMaterialIndex]->metallicRGBA->Bind(5);

		if (model->mMaterial[mMaterialIndex]->bUseRoughnessMap)
			model->mMaterial[mMaterialIndex]->mRoughnessMap->Bind(6);
		else
			model->mMaterial[mMaterialIndex]->roughnessRGBA->Bind(6);

		if (model->mMaterial[mMaterialIndex]->bUseAoMap)
			model->mMaterial[mMaterialIndex]->mAoMap->Bind(7);
		else
			Library<Texture2D>::GetInstance().GetWhiteTexture()->Bind(7);

		shader->SetInt("irradianceMap", 0);
		shader->SetInt("prefilterMap", 1);
		shader->SetInt("brdfLUT", 2);
		shader->SetInt("albedoMap", 3);
		shader->SetInt("normalMap", 4);
		shader->SetInt("metallicMap", 5);
		shader->SetInt("roughnessMap", 6);
		shader->SetInt("aoMap", 7);

		RenderCommand::DrawIndexed(mVertexArray, mIB->GetCount());
	}

	void SubMesh::Draw()
	{

		//SetupMesh(EntityID);
		static bool bInit = true;
		if (bInit)
		{
			bInit = false;

			mVertexArray->Bind();

			if (mStaticVertices.empty())
				mVB->SetData(mSkinnedVertices.data(), sizeof(StaticVertex) * mSkinnedVertices.size());
			else
				mVB->SetData(mStaticVertices.data(), sizeof(StaticVertex) * mStaticVertices.size());
			mIB->SetData(mIndices.data(), mIndices.size());

			mVertexArray->Unbind();
		}

		mVertexArray->Bind();
		RenderCommand::DrawIndexed(mVertexArray, mIB->GetCount());
		mVertexArray->Unbind();
	}

	void SubMesh::SetupMesh(int entityID)
	{
		if (mEntityID == -1)
		{
			mEntityID = entityID;
			mVertexArray->Bind();

			if (mStaticVertices.empty())
			{
				for (int i = 0; i < mStaticVertices.size(); ++i)
				{
					mStaticVertices[i].EntityID = entityID;
				}

				mVB->SetData(mSkinnedVertices.data(), sizeof(SkinnedVertex) * mSkinnedVertices.size());
			}
			else
			{
				for (int i = 0; i < mStaticVertices.size(); ++i)
				{
					mStaticVertices[i].EntityID = entityID;
				}
				mVB->SetData(mStaticVertices.data(), sizeof(StaticVertex) * mStaticVertices.size());
			}

			mIB->SetData(mIndices.data(), mIndices.size());

			mVertexArray->Unbind();
		}
	}
}