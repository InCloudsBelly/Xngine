#pragma once

#include "Runtime/EcsFramework/Component/ComponentBase.h"
#include "Runtime/Camera/SceneCamera.h"

namespace X
{
	class CameraComponent : public ComponentBase
	{
	public:
        SceneCamera Camera;
        bool Primary = true; // TODO: think about moving to Scene
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
	};
}