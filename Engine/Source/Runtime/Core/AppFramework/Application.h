#pragma once

#include "Runtime/Core/Base/Base.h"
#include "Runtime/Core/Base/PublicSingleton.h"
#include "Runtime/Core/Window.h"
#include "Runtime/Core/AppFramework/Layer/LayerStack.h"
#include "Runtime/Events/Event.h"
#include "Runtime/Events/ApplicationEvent.h"

#include "Runtime/Core/Timestep.h"

#include "Runtime/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace X
{
	class Application : public PublicSingleton<Application>
	{
	public:
		Application() = default;
		virtual ~Application() {}

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* layer);

		[[nodiscard]] Window& GetWindow() { return *mWindow; }

		void Close();

		[[nodiscard]] ImGuiLayer* GetImGuiLayer() { return mImGuiLayer; }

	private:
		void Init(const std::string& name);
		void Run();
		void Clean();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		Scope<Window> mWindow;
		ImGuiLayer* mImGuiLayer;
		bool bRunning = true;
		bool bMinimized = false;
		LayerStack mLayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		friend int ::main(int argc, char** argv);

		friend void AppInitialize(Application& app);
	};
}