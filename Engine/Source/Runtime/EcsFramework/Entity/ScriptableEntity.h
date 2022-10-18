#pragma once

#include "Runtime/EcsFramework/Entity/Entity.h"

namespace X
{
    class ScriptableEntity
    {
    public:
        virtual ~ScriptableEntity() {}

        template<typename T>
        T& GetComponent()
        {
            return mEntity.GetComponent<T>();
        }
    protected:
        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate(Timestep ts) {}
    private:
        Entity mEntity;
        friend class Level;
    };
}
