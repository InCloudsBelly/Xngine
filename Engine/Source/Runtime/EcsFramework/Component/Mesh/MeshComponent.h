#pragma once

#include "Runtime/EcsFramework/Component/ComponentBase.h"
#include "Runtime/Mesh/Mesh.h"

#include <filesystem>

namespace X
{
	class MeshComponent : public ComponentBase
	{
	public:
		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::string& path)
			: Path(path), mMesh(CreateRef<Mesh>(path))
		{
		}
		//MeshComponent(const std::filesystem::path& path)
		//	: Path(path)
		//{
		//}

		std::string Path;
		Ref<Mesh> mMesh;
	};
}