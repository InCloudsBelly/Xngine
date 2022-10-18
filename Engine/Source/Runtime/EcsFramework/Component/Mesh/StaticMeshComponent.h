#pragma once

#include "Runtime/EcsFramework/Component/ComponentBase.h"
#include "Runtime/Renderer/Model.h"

#include <filesystem>

namespace X
{
	class StaticMeshComponent : public ComponentBase
	{
	public:
		StaticMeshComponent() = default;
		StaticMeshComponent(const StaticMeshComponent&) = default;
		StaticMeshComponent(const std::string& path)
			: Path(path)
		{
		}
		StaticMeshComponent(const std::filesystem::path& path)
			: Path(path)
		{
		}

		Model Mesh;
		std::filesystem::path Path;
	};
}