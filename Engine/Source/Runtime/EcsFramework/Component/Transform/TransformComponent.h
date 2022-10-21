#pragma once

#include "Runtime/EcsFramework/Component/ComponentBase.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace X
{
	class TransformComponent : public ComponentBase
	{
	public:
        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3 & translation)
            : Translation(translation) {}

        [[nodiscard]] glm::mat4 GetTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

            return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
        }

        [[nodiscard]] glm::vec3 GetTranslation() const { return Translation; }

        glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };// Euler angles
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
	};
}