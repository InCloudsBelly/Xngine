#pragma once

#include "Runtime/Core/Base/Base.h"
#include "Runtime/Core/Timestep.h"
#include "Runtime/Events/Event.h"

namespace X
{

	class XNGINE_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		[[nodiscard]] inline const std::string& GetName() const { return mDebugName; }
	protected:
		std::string mDebugName;
	};

}