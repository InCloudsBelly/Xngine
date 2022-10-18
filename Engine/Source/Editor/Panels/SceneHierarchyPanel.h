#pragma once

#include "Runtime/Core/Base/Base.h"
#include "Runtime/Scene/Scene.h"
#include "Runtime/Scene/Entity.h"

namespace X
{
    class SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& context);

        void SetContext(const Ref<Scene>& context);

        void OnImGuiRender(bool* pOpen, bool* pOpenProperties);

		Entity GetSelectedEntity() const { return mSelectionContext; }
		void SetSelectedEntity(Entity entity);
    private:
        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity);
    private:
        Ref<Scene> mContext;
        Entity mSelectionContext;
    };
}
