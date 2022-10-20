#pragma once

#include "Xpch.h"

#include "Runtime/Core/Base/Base.h"
#include "Runtime/Events/Event.h"
#include "Runtime/Renderer/GraphicsContext.h"

namespace X
{

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Xngine",
			uint32_t width = 1600,
			uint32_t height = 900)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// Interface representing a desktop system based Window
	class XNGINE_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		[[nodiscard]] virtual uint32_t GetWidth() const = 0;
		[[nodiscard]] virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		[[nodiscard]] virtual void* GetNativeWindow() const = 0;
		[[nodiscard]] virtual Ref<GraphicsContext> GetGraphicsContext() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};

}