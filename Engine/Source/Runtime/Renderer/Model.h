#pragma once

#include "Runtime/Core/Base/Base.h"

#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/StaticMesh.h"
#include "Runtime/Renderer/Texture.h"
#include "Runtime/Renderer/Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <optional>

namespace X
{
	class Model
	{
	public:
		Model() = default;
		Model(const std::string& path)
			: mMaterial(CreateRef<Material>(ShaderLibrary::GetDefaultShader()))
		{
			LoadModel(path);
		}

		Model(const std::string& path, Ref<Shader> shader)
			: mMaterial(CreateRef<Material>(shader))
		{
			LoadModel(path);
		}

		void SetShader(Ref<Shader> shader) { mMaterial->SetShader(shader); };
		void Draw(const glm::mat4& transform, const glm::vec3& cameraPos, int entityID);
		void Draw(const glm::mat4& transform, const glm::vec3& cameraPos, Ref<Shader> shader, int entityID);

		void Draw();

	private:
		Ref<Material> mMaterial = CreateRef<Material>();
		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		StaticMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		std::optional<std::vector<MaterialTexture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type);

	private:
		std::vector<StaticMesh> mMeshes;
		std::string mDirectory;
	};
}
