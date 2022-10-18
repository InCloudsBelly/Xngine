#pragma once

#include "Runtime/Core/UUID.h"
#include "Runtime/EcsFramework/Level/Level.h"

#include <entt.hpp>

namespace X
{
    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity handle, Level* level);
        Entity(const Entity& other) = default;

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            X_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            T& component = mLevel->mRegistry.emplace<T>(mEntityHandle, std::forward<Args>(args)...);
            mLevel->OnComponentAdded<T>(*this, component);
            return component;
        }

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = mLevel->mRegistry.emplace_or_replace<T>(mEntityHandle, std::forward<Args>(args)...);
			mLevel->OnComponentAdded<T>(*this, component);
			return component;
		}

        template<typename T>
        T& GetComponent()
        {
            X_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return mLevel->mRegistry.get<T>(mEntityHandle);
        }

        template<typename T>
        bool HasComponent()
        {
            return mLevel->mRegistry.all_of<T>(mEntityHandle);
        }

        template<typename T>
        void RemoveComponent()
        {
            X_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            mLevel->mRegistry.remove<T>(mEntityHandle);
        }

        operator bool() const { return mEntityHandle != entt::null; }
        operator entt::entity() const { return mEntityHandle; }
        operator uint32_t() const { return (uint32_t)mEntityHandle; }

        UUID GetUUID();
        const std::string& GetName();

        bool operator==(const Entity& other) const 
        { 
            return mEntityHandle == other.mEntityHandle && mLevel == other.mLevel; 
        }

        bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }
    private:
        entt::entity mEntityHandle{ entt::null };
        Level* mLevel = nullptr;
    };
}
