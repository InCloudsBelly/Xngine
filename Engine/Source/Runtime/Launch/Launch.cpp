#include "Xpch.h"
#include "Runtime/Core/Application.h"

namespace X
{
	extern void AppInitialize(Application& app);
}

int main(int argc, char** argv)
{
	X::AppInitialize(X::Application::GetInstance());
	X::Application::GetInstance().Run();
	X::Application::GetInstance().Clean();
}
