#include "Xpch.h"
#include "Entity.h"

namespace X
{
    Entity::Entity(entt::entity handle, Scene* scene)
        : m_EntityHandle(handle), m_Scene(scene)
    {
    }
}