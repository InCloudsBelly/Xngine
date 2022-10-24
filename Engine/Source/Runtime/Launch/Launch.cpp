#include "Xpch.h"
#include "Runtime/Core/AppFramework/Application.h"
#include <pybind11/embed.h>

namespace X
{
	extern void AppInitialize(Application& app);
}

int main(int argc, char** argv)
{
	// https://github.com/pybind/pybind11/issues/3112
	// we should only have one interpreter in our program
	pybind11::scoped_interpreter guard{};

	X::AppInitialize(X::Application::GetInstance());
	X::Application::GetInstance().Run();
	X::Application::GetInstance().Clean();
}
