#include "EditorLayer.h"

#include <Xngine.h>
#include <Runtime/Core/EntryPoint.h>	 

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace X
{
	class HEngineEditor : public Application
	{
	public:
		HEngineEditor()
			: Application("X Editor")
		{
			PushLayer(new EditorLayer());
		}

		~HEngineEditor()
		{
		}
	};

	Application* CreateApplication()
	{
		return new HEngineEditor();
	}
}
