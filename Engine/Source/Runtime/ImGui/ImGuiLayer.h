#pragma once

#include "Runtime/Core/AppFramework/Layer/Layer.h""

#include "Runtime/Events/ApplicationEvent.h"
#include "Runtime/Events/KeyEvent.h"
#include "Runtime/Events/MouseEvent.h"

namespace X
{
    class XNGINE_API ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnEvent(Event& e) override;

        void Begin();
        void End();

        void BlockEvents(bool block) { mBlockEvents = block; }

        void SetDarkThemeColors();
    private:
        bool mBlockEvents = true;
    };
}
