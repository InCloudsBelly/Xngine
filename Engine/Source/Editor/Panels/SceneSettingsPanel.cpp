#include "Editor/Panels/SceneSettingsPanel.h"
#include "Editor/IconManager/IconManager.h"
#include "Editor/ImGuiWrapper/ImGuiWrapper.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Library/Library.h"

#include <imgui/imgui.h>
#include <regex>


namespace X
{
	SceneSettingsPanel::SceneSettingsPanel()
	{
		Ref<Texture> mDefaultTexture = IconManager::GetInstance().GetNullTexture();

		mPaths = std::vector<std::string>(6);
		mRight = mDefaultTexture;
		mLeft = mDefaultTexture;
		mTop = mDefaultTexture;
		mBottom = mDefaultTexture;
		mFront = mDefaultTexture;
		mBack = mDefaultTexture;
	}

	// Still some bugs (top bottom inverse?), see https://stackoverflow.com/questions/55558241/opengl-cubemap-face-order-sampling-issue
	void SceneSettingsPanel::OnImGuiRender(bool* pOpen)
	{
		if (!ImGui::Begin("Scene Settings", pOpen))
		{
			ImGui::End();
			return;
		}

		const char* modes[] = { "None", "Environment Hdr", "Sky Box" };
		ImGui::Text("Mode");
		ImGui::SameLine();
		if (ImGui::Combo("##Mode", (int*)(&ModeManager::mSceneMode), modes, IM_ARRAYSIZE(modes)))
		{

		}

		const auto& SkyBoxTreeNode = [&mPaths = mPaths](const char* nodeName, Ref<Texture>& tex, uint32_t pathIndex) {
			if (ImGui::TreeNode(nodeName))
			{
				ImGui::Image((ImTextureID)tex->GetRendererID(), ImVec2(64, 64), ImVec2{ 0,1 }, ImVec2{ 1,0 });
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						auto path = (const wchar_t*)payload->Data;
						std::string relativePath = (std::filesystem::path("Assets") / path).string();
						std::filesystem::path texturePath = ConfigManager::GetInstance().GetAssetsFolder() / path;
						mPaths[pathIndex] = texturePath.string();
						relativePath = std::regex_replace(relativePath, std::regex("\\\\"), "/");
						tex = IconManager::GetInstance().LoadOrFindTexture(relativePath);
						Library<CubeMapTexture>::GetInstance().Get("SkyBoxTexture")->SetFace((FaceTarget)pathIndex, relativePath);
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::TreePop();
			}
		};


		//Sky Box
		if (ImGuiWrapper::TreeNodeExStyle1((void*)"SkyBox", "SkyBox")) 
		{
			if (ImGui::Button("Update"))
			{
				Library<CubeMapTexture>::GetInstance().Get("SkyBoxTexture")->Generate();
			}
			SkyBoxTreeNode("Right +X", mRight, 0);
			SkyBoxTreeNode("Left -X", mLeft, 1);
			SkyBoxTreeNode("Top +Y", mTop, 2);
			SkyBoxTreeNode("Bottom -Y", mBottom, 3);
			SkyBoxTreeNode("Front +Z", mFront, 4);
			SkyBoxTreeNode("Back -Z", mBack, 5);
			ImGui::TreePop();
		}
		ImGui::End();
	}

}
