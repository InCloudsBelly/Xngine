#include "Xpch.h"
#include "Entity.h"

namespace X
{
    Entity::Entity(entt::entity handle, Scene* scene)
        : mEntityHandle(handle), mScene(scene)
    {
    }
}