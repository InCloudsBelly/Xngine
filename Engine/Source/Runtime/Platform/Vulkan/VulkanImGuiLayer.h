#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "Runtime/ImGui/ImGuiLayer.h"

namespace X
{
	class VulkanImGuiLayer : public ImGuiLayer
	{
	public:
		VulkanImGuiLayer() = default;
		VulkanImGuiLayer(const std::string& name) : ImGuiLayer(name) {}
		virtual ~VulkanImGuiLayer();

		virtual void Begin() override;
		virtual void End() override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

	private:
		float mTime = 0.0f;

	};
}