#include "Editor/EditorLayer.h"
#include "Editor/ImGuiWrapper/ImGuiWrapper.h"
#include "Editor/IconManager/IconManager.h"
#include "Runtime/EcsFramework/Serializer/SceneSerializer.h"
#include "Runtime/Utils/PlatformUtils.h"
#include "Runtime/Utils/MathUtils/MathUtils.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <ImGuizmo.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace X
{
	namespace Utils
	{
		// from https://github.com/Acmen-Team/Epoch/tree/dev
		template<typename UIFunction>
		static void SceneToolbar(ImGuiDockNode* node, const float DISTANCE, int* corner, UIFunction uiFunc)
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

			ImVec2 work_area_pos = node->Pos;
			ImVec2 work_area_size = node->Size;

			if (*corner != -1)
			{
				window_flags |= ImGuiWindowFlags_NoMove;
				ImGuiViewport* viewport = ImGui::GetMainViewport();

				ImVec2 window_pos = ImVec2((*corner & 1) ? (work_area_pos.x + work_area_size.x - DISTANCE) : (work_area_pos.x + DISTANCE), (*corner & 2) ? (work_area_pos.y + work_area_size.y - DISTANCE) : (work_area_pos.y + DISTANCE));
				ImVec2 window_pos_pivot = ImVec2((*corner & 1) ? 1.0f : 0.0f, (*corner & 2) ? 1.0f : 0.0f);
				ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
				ImGui::SetNextWindowViewport(node->ID);
			}
			ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background

			uiFunc(corner, work_area_size, window_flags);
		}
	}

	// Window
	static bool bShowViewport = true;
	static bool bShowContentBrowser = true;
	static bool bShowSceneHierachy = true;
	static bool bShowProperties = true;
	static bool bShowStats = false;
	static bool bShowEngineSettings = true;
	static bool bShowSceneSettings = true;
	static bool bShowSRT = true;

	// Help
	static bool bShowTutorial = false;
	static bool bShowAboutMe = false;
	static bool bShowDemoImGui = false;

    EditorLayer::EditorLayer()
		: Layer("EditorLayer")
    {
    }

    void EditorLayer::OnAttach()
    {
		mShowPass = Renderer3D::GeometryPipeline->GetSpecification().RenderPass;
		mShowFramebuffer = mShowPass->GetSpecification().TargetFramebuffer;

		mActiveScene = CreateRef<Level>();

		mEditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
    }

    void EditorLayer::OnDetach()
    {
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        X_PROFILE_FUNCTION();

		// Resize
		if (FramebufferSpecification spec = mShowFramebuffer->GetSpecification();
			ConfigManager::mViewportSize.x > 0.0f && ConfigManager::mViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != ConfigManager::mViewportSize.x || spec.Height != ConfigManager::mViewportSize.y))
		{
			mShowFramebuffer->Resize((uint32_t)ConfigManager::mViewportSize.x, (uint32_t)ConfigManager::mViewportSize.y);
			mEditorCamera.SetViewportSize(ConfigManager::mViewportSize.x, ConfigManager::mViewportSize.y);
			mActiveScene->OnViewportResize((uint32_t)ConfigManager::mViewportSize.x, (uint32_t)ConfigManager::mViewportSize.y);
			PostProcessing::mFramebuffer->Resize((uint32_t)ConfigManager::mViewportSize.x, (uint32_t)ConfigManager::mViewportSize.y);
			Renderer3D::QuadPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer->Resize((uint32_t)ConfigManager::mViewportSize.x, (uint32_t)ConfigManager::mViewportSize.y);
			Renderer3D::HBAOGbufferPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer->Resize((uint32_t)ConfigManager::mViewportSize.x, (uint32_t)ConfigManager::mViewportSize.y);
			Renderer3D::HBAOPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer->Resize((uint32_t)ConfigManager::mViewportSize.x, (uint32_t)ConfigManager::mViewportSize.y);
			Renderer3D::HBAOBlurPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer->Resize((uint32_t)ConfigManager::mViewportSize.x, (uint32_t)ConfigManager::mViewportSize.y);
			Renderer3D::HBAOQuadPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer->Resize((uint32_t)ConfigManager::mViewportSize.x, (uint32_t)ConfigManager::mViewportSize.y);
		}

        // Render
        Renderer2D::ResetStats();
        
		mShowFramebuffer->Bind();
		
		RenderCommand::SetClearColor({ 0.4f, 0.4f, 0.4f, 1 });
		RenderCommand::Clear();

		// Clear out entity ID attachment to -1
		mShowFramebuffer->ClearAttachment(1, -1);

		if (ModeManager::IsEditState())
		{
			mEditorCamera.OnUpdate(ts);

			mActiveScene->OnUpdateEditor(ts, mEditorCamera);

		}
		else
		{
			mActiveScene->OnUpdateRuntime(ts);
		}

		auto [mx, my] = ImGui::GetMousePos();
		mx -= mViewportBounds[0].x;
		my -= mViewportBounds[0].y;
		glm::vec2 viewportSize = mViewportBounds[1] - mViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = mShowFramebuffer->ReadPixel(1, mouseX, mouseY);
			mHoveredEntity = pixelData == -1 ? Entity{} : Entity{ (entt::entity)pixelData, mActiveScene.get() };
		}

		Entity selectedEntity = mSceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
		{
			ConfigManager::selectedEntity = (int)(uint32_t)selectedEntity;
			if (Input::IsKeyPressed(X_KEY_F))
				mEditorCamera.SetCenter(selectedEntity.GetComponent<TransformComponent>().Translation);
		}
		else
		{
			ConfigManager::selectedEntity = -1;
		}


		//OnOverlayRender();

		mShowFramebuffer->Unbind();
    }

    void EditorLayer::OnImGuiRender()
    {
		static bool bChangeDim = false;

		// ----DockSpace Begin----
        static bool dockspaceOpen = true;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 110.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        style.WindowMinSize.x = minWinSizeX;

		// ----MenuBar Begin----
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                    NewScene();

                if (ImGui::MenuItem("Open...", "Ctrl+O"))
                    OpenScene();

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                    SaveSceneAs();

                if (ImGui::MenuItem("Exit", NULL, false)) 
					Application::GetInstance().Close();

                ImGui::EndMenu();
            }
			if (ImGui::BeginMenu("Window"))
			{
				ImGui::MenuItem("Viewport", NULL, &bShowViewport);
				ImGui::MenuItem("Content Browser", NULL, &bShowContentBrowser);
				ImGui::MenuItem("Level Hierachy", NULL, &bShowSceneHierachy);
				ImGui::MenuItem("Properties", NULL, &bShowProperties);
				ImGui::MenuItem("Stats", NULL, &bShowStats);
				ImGui::MenuItem("Engine Settings", NULL, &bShowEngineSettings);
				ImGui::MenuItem("Environment Settings", NULL, &bShowSceneSettings);

				if (ImGui::MenuItem("Load Default Layout"))
					LoadDefaultEditorConfig();

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				ImGui::MenuItem("Tutorial", NULL, &bShowTutorial);
				ImGui::MenuItem("About Me", NULL, &bShowAboutMe);
				ImGui::MenuItem("Demo ImGui", NULL, &bShowDemoImGui);

				ImGui::EndMenu();
			}

            ImGui::EndMenuBar();
        }
		// ----MenuBar End----

		// ----Windows Begin----
		if (bShowContentBrowser)
		{
			mContentBrowserPanel.OnImGuiRender(&bShowContentBrowser);
		}
		if (bShowSceneHierachy || bShowProperties)
		{
			mSceneHierarchyPanel.OnImGuiRender(&bShowSceneHierachy, &bShowProperties);
		}
		if (bShowStats)
		{
			ImGui::Begin("Stats", &bShowStats);

			std::string name = "None";
			if (mHoveredEntity)
				name = mHoveredEntity.GetComponent<TagComponent>().Tag;
			ImGui::Text("Hovered Entity: %s", name.c_str());

			auto stats = Renderer2D::GetStats();
			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quads: %d", stats.QuadCount);
			ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
			ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

			ImGui::End();
		}
		if (bShowEngineSettings)
		{
			ImGui::Begin("Engine Settings", &bShowEngineSettings);
			const char* modes[] = { "2D", "3D" };
			int lastMode = ModeManager::b3DMode;
			ImGui::Columns(2, nullptr, false);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text("Mode");
			ImGui::NextColumn();
			if (ImGui::Combo("##Mode", &ModeManager::b3DMode, modes, IM_ARRAYSIZE(modes)))
			{
				if (lastMode != ModeManager::b3DMode)
				{
					bChangeDim = true;
				}
			}
			ImGui::EndColumns();

			ImGui::Columns(2, nullptr, false);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text("Camera Speed");
			ImGui::NextColumn();
			ImGui::SliderFloat("##Camera Speed", &mEditorCamera.mCameraSpeed, 0.1f, 10.0f);
			ImGui::EndColumns();

			if (ImGuiWrapper::TreeNodeExStyle1((void*)"Physics Settings", "Physics Settings"))
			{
				ImGui::Checkbox("Show physics colliders", &mShowPhysicsColliders);
				ImGui::TreePop();
			}

			if (ImGuiWrapper::TreeNodeExStyle1((void*)"Post Processing", "Post Processing"))
			{
				if (ImGui::Button("Add Post Processing"))
					ImGui::OpenPopup("AddPostProcessing");

				if (ImGui::BeginPopup("AddPostProcessing"))
				{
					if (ImGui::MenuItem("Outline"))
					{
						mShowPass->AddPostProcessing(PostProcessingType::Outline);
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::MenuItem("Cartoon"))
					{
						mShowPass->AddPostProcessing(PostProcessingType::Cartoon);
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::MenuItem("GrayScale"))
					{
						mShowPass->AddPostProcessing(PostProcessingType::GrayScale);
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::MenuItem("GaussianBlur"))
					{
						mShowPass->AddPostProcessing(PostProcessingType::GaussianBlur);
						ImGui::CloseCurrentPopup();
					}
					if (ImGui::MenuItem("ComputeTest"))
					{
						mShowPass->AddPostProcessing(PostProcessingType::ComputeTest);
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				size_t postPassBegin;
				if (mShowFramebuffer->GetSpecification().Samples > 1)
					postPassBegin = 1;
				else
					postPassBegin = 0;

				for (size_t i = postPassBegin; i < mShowPass->GetPostProcessings().size(); i++)
				{
					ImGui::Selectable(PostProcessing::PostTypeToString(mShowPass->GetPostProcessings()[i]->mType).c_str());
					
					// imgui demo: Drag to reorder items (simple)
					if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
					{
						int next = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
						if (next >= 1 && next < mShowPass->GetPostProcessings().size())
						{
							PostProcessingType tempType = mShowPass->GetPostProcessings()[i]->mType;
							mShowPass->GetPostProcessings()[i]->mType = mShowPass->GetPostProcessings()[next]->mType;
							mShowPass->GetPostProcessings()[next]->mType = tempType;
							ImGui::ResetMouseDragDelta();
						}
					}

					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Delete"))
							mShowPass->GetPostProcessings().erase(mShowPass->GetPostProcessings().begin() + i);

						ImGui::EndPopup();
					}
				}

				ImGui::TreePop();
			}

			ImGui::End();
		}
		if (bShowSceneSettings)
		{
			mSceneSettingsPanel.OnImGuiRender(&bShowSceneSettings);

		}
		if (bShowViewport)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Viewport");
			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();
			mViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			mViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			mViewportFocused = ImGui::IsWindowFocused();
			mViewportHovered = ImGui::IsWindowHovered();
			Application::GetInstance().GetImGuiLayer()->BlockEvents(!mViewportFocused && !mViewportHovered);

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			ConfigManager::mViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			uint32_t textureID;
			uint32_t AttachIndex = 0; // By changing this can we select which ColorAttachment to show	;

			Ref<Framebuffer> gBufferTestfb = Renderer3D::HBAOQuadPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer;
			textureID = gBufferTestfb->GetColorAttachmentRendererID(AttachIndex);
			//textureID = gBufferTestfb->GetDepthAttachmentRendererID();


			//if (mShowFramebuffer->GetSpecification().Samples > 1 || mShowPass->GetPostProcessings().size() > 0) // At least have MSAA post processing;
			//	textureID = mShowPass->ExcuteAndReturnFinalTex(AttachIndex);
			//else
			//	textureID = mShowFramebuffer->GetColorAttachmentRendererID(AttachIndex);

			ImGui::Image((ImTextureID)textureID, ImVec2{ ConfigManager::mViewportSize.x, ConfigManager::mViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					OpenScene(std::filesystem::path(ConfigManager::GetInstance().GetAssetsFolder()) / path);
				}
				ImGui::EndDragDropTarget();
			}

			// Gizmos
			Entity selectedEntity = mSceneHierarchyPanel.GetSelectedEntity();
			if (selectedEntity && mGizmoType != -1)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();

				ImGuizmo::SetRect(mViewportBounds[0].x, mViewportBounds[0].y, mViewportBounds[1].x - mViewportBounds[0].x, mViewportBounds[1].y - mViewportBounds[0].y);

				// Camera
				// Runtime camera from entity
				/*auto cameraEntity = mActiveScene->GetPrimaryCameraEntity();
				const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
				const glm::mat4& cameraProjection = camera.GetProjection();
				glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());*/

				// Editor camera
				const glm::mat4& cameraProjection = mEditorCamera.GetProjection();
				glm::mat4 cameraView = mEditorCamera.GetViewMatrix();

				// Entity transform
				auto& tc = selectedEntity.GetComponent<TransformComponent>();
				glm::mat4 transform = tc.GetTransform();

				// Snapping
				bool snap = Input::IsKeyPressed(Key::LeftControl);
				float snapValue = 0.5f; // Snap to 0.5m for translation/scale
				// Snap to 45 degrees for rotation
				if (mGizmoType == ImGuizmo::OPERATION::ROTATE)
					snapValue = 45.0f;

				float snapValues[3] = { snapValue, snapValue, snapValue };

				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
					(ImGuizmo::OPERATION)mGizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
					nullptr, snap ? snapValues : nullptr);

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 translation, rotation, scale;
					Math::DecomposeTransform(transform, translation, rotation, scale);

					glm::vec3 deltaRotation = rotation - tc.Rotation;
					tc.Translation = translation;
					tc.Rotation += deltaRotation;
					tc.Scale = scale;
				}
			}

			static int transCorner = 1;
			ImGuiDockNode* node = ImGui::GetWindowDockNode();
			node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

			Utils::SceneToolbar(node, 10.0f, &transCorner, [&](int* corner, const ImVec2& work_area_size, const ImGuiWindowFlags m_window_flags) {

				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

				if (ImGui::Begin("SRT Toolbar", &bShowSRT, m_window_flags))
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.1f, 0.1f, 0.1f, 0.5f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

					if (ImGui::ImageButton((void*)IconManager::GetInstance().Get("TransIcon")->GetRendererID(), ImVec2(30.0f, 30.0f), ImVec2(0, 1), ImVec2(1, 0), 0.0f, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), mGizmoType == ImGuizmo::OPERATION::TRANSLATE ? ImVec4(1.0f, 1.0f, 0.0f, 0.9f) : ImVec4(1.0f, 1.0f, 0.0f, 0.2f)))
					{
						mGizmoType = ImGuizmo::OPERATION::TRANSLATE;
					}
					ImGui::SameLine();
					if (ImGui::ImageButton((void*)IconManager::GetInstance().Get("RotateIcon")->GetRendererID(), ImVec2(30.0f, 30.0f), ImVec2(0, 1), ImVec2(1, 0), 0.0f, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), mGizmoType == ImGuizmo::OPERATION::ROTATE ? ImVec4(1.0f, 1.0f, 0.0f, 0.9f) : ImVec4(1.0f, 1.0f, 0.0f, 0.2f)))
					{
						mGizmoType = ImGuizmo::OPERATION::ROTATE;
					}
					ImGui::SameLine();
					if (ImGui::ImageButton((void*)IconManager::GetInstance().Get("ScaleIcon")->GetRendererID(), ImVec2(30.0f, 30.0f), ImVec2(0, 1), ImVec2(1, 0), 0.0f, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), mGizmoType == ImGuizmo::OPERATION::SCALE ? ImVec4(1.0f, 1.0f, 0.0f, 0.9f) : ImVec4(1.0f, 1.0f, 0.0f, 0.2f)))
					{
						mGizmoType = ImGuizmo::OPERATION::SCALE;
					}

					ImGui::PopStyleColor(3);

				}

				ImGui::End();
				ImGui::PopStyleVar();
				});


			ImGui::End();
			ImGui::PopStyleVar();
		}
		// ----Windows End----


		// ----Help Begin----
		// TODO
		ImGuiWindowFlags helpMenuFlags = ImGuiWindowFlags_NoDocking;
		if (bShowTutorial)
		{
			ImGui::Begin("Tutorial", &bShowTutorial, helpMenuFlags);
			ImGui::Text("Hello!");
			ImGui::Text("Hello!");
			ImGui::Text("Hello!");
			ImGui::Text("Hello!");
			ImGui::Text("Hello!");
			ImGui::Text("Hello!");
			ImGui::Text("Hello!");
			ImGui::Text("Hello!");
			ImGui::Text("Hello!");
			ImGui::Text("Hello!");
			ImGui::End();
		}
		if (bShowAboutMe)
		{
			ImGui::Begin("About Me", &bShowAboutMe, helpMenuFlags);
			ImGui::Text("My name is zxh!");
			ImGui::End();
		}
		if (bShowDemoImGui)
		{
			ImGui::ShowDemoWindow(&bShowDemoImGui);
		}
		// ----Help End----

		UI_Toolbar();

        ImGui::End(); 
		// ----DockSpace End----

		if (bChangeDim)
		{
			mActiveScene->ChangeDimMode();
			bChangeDim = false;
		}
    }

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGuiDockNode* node = ImGui::GetWindowDockNode();
		node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

		float size = ImGui::GetWindowHeight() - 4.0f;
		Ref<Texture2D> icon = ModeManager::IsEditState() ? IconManager::GetInstance().GetPlayIcon() : IconManager::GetInstance().GetStopIcon();
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (ModeManager::IsEditState())
				OnScenePlay();
			else 
				OnSceneStop();
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::LoadDefaultEditorConfig()
	{
		const std::filesystem::path CurrentEditorConfigPath{ AssetManager::GetFullPath("imgui.ini") };
		const std::filesystem::path DefaultEditorConfigPath{ AssetManager::GetFullPath("Config/imgui.ini") };

		X_CORE_ASSERT(std::filesystem::exists(DefaultEditorConfigPath));
		if (std::filesystem::exists(CurrentEditorConfigPath))
			std::filesystem::remove(CurrentEditorConfigPath);
		std::filesystem::copy(DefaultEditorConfigPath, std::filesystem::current_path());

		bShowViewport = true;
		bShowContentBrowser = true;
		bShowSceneHierachy = true;
		bShowProperties = true;
		bShowStats = false;
		bShowEngineSettings = true;
		bShowSceneSettings = true;
		bShowSRT = true;

		// Help
		bShowTutorial = false;
		bShowAboutMe = false;
		bShowDemoImGui = false;


		// seems imgui docking branch has some bugs with load ini file?

		//auto& io = ImGui::GetIO();
		//io.IniFilename = DefaultEditorConfigPath.string().c_str();
		//ImGui::LoadIniSettingsFromDisk(DefaultEditorConfigPath.string().c_str());
		//ImGui::DockContextRebuildNodes(ImGui::GetCurrentContext());
	}

    void EditorLayer::OnEvent(Event& e)
    {
		mEditorCamera.OnEvent(e);

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(X_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(X_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {
        // Shortcuts
        if (e.GetRepeatCount() > 0)
            return false;

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		bool mouseRight = Input::IsMouseButtonPressed(Mouse::ButtonRight);
        switch (e.GetKeyCode())
        {
            case Key::N:
            {
                if (control)
                    NewScene();
                break;
            }
            case Key::O:
            {
                if (control)
                    OpenScene();
                break;
            }
            case Key::S:
            {
				if (control)
				{
					if (shift)
						SaveSceneAs();
					else
						SaveScene();
				}
                break;
            }

			// Level Commands
			case Key::D:
			{
				if (control)
					OnDuplicateEntity();
				break;
			}

			// Gizmos
			case Key::Q:
				if (!mouseRight)
					mGizmoType = -1;
				break;
			case Key::W:
				if (!mouseRight)
					mGizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case Key::E:
				if (!mouseRight)
					mGizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			case Key::R:
				if (!mouseRight)
					mGizmoType = ImGuizmo::OPERATION::SCALE;
				break;
        }
		return false;
    }

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (mViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt) && mHoveredEntity)
				mSceneHierarchyPanel.SetSelectedEntity(mHoveredEntity);
		}
		return false;
	}

	void EditorLayer::OnOverlayRender()
	{
		if (ModeManager::IsEditState())
		{
			Renderer2D::BeginScene(mEditorCamera);
		}
		else
		{
			Entity camera = mActiveScene->GetPrimaryCameraEntity();
			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
		}

		if (mShowPhysicsColliders)
		{
			{
				auto view = mActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (auto entity : view)
				{
					auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::scale(glm::mat4(1.0f), scale);

					//glm::mat4 transform = glm::translate(tc.GetTransform(), glm::vec3(0, 0, 0.01f));

					Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
				}
			}

			{
				auto view = mActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
				for (auto entity : view)
				{
					auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::scale(glm::mat4(1.0f), scale);

					//glm::mat4 transform = glm::translate(tc.GetTransform(), glm::vec3(0, 0, 0.01f));

					Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.05f);
				}
			}

			Renderer2D::EndScene();
		}
	}

    void EditorLayer::NewScene()
    {
        mActiveScene = CreateRef<Level>();
		mActiveScene->OnViewportResize((uint32_t)ConfigManager::mViewportSize.x, (uint32_t)ConfigManager::mViewportSize.y);
		mSceneHierarchyPanel.SetContext(mActiveScene);

		mEditorScenePath = std::filesystem::path();
    }

    void EditorLayer::OpenScene()
    {
        std::string filepath = FileDialogs::OpenFile("X Level (*.scene)\0*.scene\0");
		if (!filepath.empty())
			OpenScene(filepath);
    }

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (!ModeManager::IsEditState())
		{
			OnSceneStop();
			ModeManager::ChangeState();
		}

		if (path.extension().string() != ".scene")
		{
			X_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		Ref<Level> newScene = CreateRef<Level>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			mEditorScene = newScene;
			mEditorScene->OnViewportResize((uint32_t)ConfigManager::mViewportSize.x, (uint32_t)ConfigManager::mViewportSize.y);
			mSceneHierarchyPanel.SetContext(mEditorScene);

			mActiveScene = mEditorScene;
			mEditorScenePath = path;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!mEditorScenePath.empty())
			SerializeScene(mActiveScene, mEditorScenePath);
		else
			SaveSceneAs();
	}

    void EditorLayer::SaveSceneAs()
    {
        std::string filepath = FileDialogs::SaveFile("X Level (*.scene)\0*.scene\0");
        if (!filepath.empty())
        {
			SerializeScene(mActiveScene, filepath);
			mEditorScenePath = filepath;
        }
    }

	void EditorLayer::SerializeScene(Ref<Level> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	void EditorLayer::OnScenePlay()
	{
		ModeManager::ChangeState();

		mActiveScene = Level::Copy(mEditorScene);
		mActiveScene->OnRuntimeStart();

		mSceneHierarchyPanel.SetContext(mActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		ModeManager::ChangeState();

		mActiveScene->OnRuntimeStop();
		mActiveScene = mEditorScene;

		mSceneHierarchyPanel.SetContext(mActiveScene);
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (!ModeManager::IsEditState())
			return;

		Entity selectedEntity = mSceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
			mEditorScene->DuplicateEntity(selectedEntity);
	}
}
