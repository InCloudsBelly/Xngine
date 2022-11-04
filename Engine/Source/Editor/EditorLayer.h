#pragma once

#include "Xngine.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneSettingsPanel.h"

#include "Runtime/Camera/EditorCamera.h"
#include "Runtime/Renderer/RenderPass.h"

namespace X
{
    class EditorLayer : public Layer
    {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        void OnUpdate(Timestep ts) override;
        virtual void OnImGuiRender() override;
        void OnEvent(Event& e) override;
    private:
        bool OnKeyPressed(KeyPressedEvent& e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

        void OnOverlayRender();

        void NewScene();
        void OpenScene();
        void OpenScene(const std::filesystem::path& path);
        void SaveScene();
        void SaveSceneAs();

        void SerializeScene(Ref<Level> scene, const std::filesystem::path& path);

        void OnScenePlay();
        void OnSceneStop();

        void OnDuplicateEntity();

        // UI Panels
        void UI_Toolbar();

        void LoadDefaultEditorConfig();
    private:
        // Temp
        Ref<VertexArray> mSquareVA;
        Ref<Shader> mFlatColorShader;
        Ref<Framebuffer> mShowFramebuffer;

        Ref<RenderPass> mShowPass;

        Ref<Level> mActiveScene;
        Ref<Level> mEditorScene;
        std::filesystem::path mEditorScenePath;
        Entity mSquareEntity;
        Entity mCameraEntity;
        Entity mSecondCamera;

        Entity mHoveredEntity;

        bool mPrimaryCamera = true;

        EditorCamera mEditorCamera;

        bool mViewportFocused = false, mViewportHovered = false;
        glm::vec2 mViewportBounds[2];

        glm::vec4 mSquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

        int mGizmoType = -1;

        bool mShowPhysicsColliders = false;

        // Panels
        SceneHierarchyPanel mSceneHierarchyPanel;
        ContentBrowserPanel mContentBrowserPanel;
        SceneSettingsPanel mSceneSettingsPanel;

    };
}

