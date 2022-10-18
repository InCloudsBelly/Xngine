#pragma once
#include "Runtime/EcsFramework/System/System.h"
#include "Runtime/EcsFramework/Level/Level.h"

namespace X
{
    class RenderSystem2D : public System
    {
    public:
        RenderSystem2D(Level * level) : System(level) {}
        virtual ~RenderSystem2D() = default;

    public:
        void OnUpdateRuntime(Timestep ts) override;
        void OnUpdateEditor(Timestep ts, EditorCamera& camra) override;
    };
}