#include "EditorLayer.h"

#include <Xngine.h>

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace X
{
	void AppInitialize(Application& app)
	{
		app.Init("Xngine Editor");
		app.PushLayer(new EditorLayer());
	}

}
