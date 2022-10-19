#pragma once

#include "Runtime/EcsFramework/Component/ComponentBase.h"

namespace X
{
	class Rigidbody2DComponent : public ComponentBase
	{
	public:
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};
}