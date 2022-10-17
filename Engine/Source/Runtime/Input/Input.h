#pragma once

#include <glm/glm.hpp>

#include "Runtime/Input/KeyCodes.h"
#include "Runtime/Input/MouseCodes.h"

namespace X
{
    class XNGINE_API Input
    {
    public:
        static bool IsKeyPressed(KeyCode key);

        static bool IsMouseButtonPressed(MouseCode button);
        static glm::vec2 GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();
    };
}