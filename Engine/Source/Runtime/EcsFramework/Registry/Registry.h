#pragma once

#include "Runtime/EcsFramework/Component/ComponentGroup.h"

#include <entt.hpp>

namespace X
{
    class Registry final
    {
    public:
        Registry() = default;
        ~Registry() = default;
    public:

    private:
        entt::registry mRegistry;
    };
}