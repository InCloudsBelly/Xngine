#pragma once

#ifdef X_PLATFORM_WINDOWS

extern X::Application* X::CreateApplication();

int main(int argc, char** argv)
{
	X::Log::Init();

	auto app = X::CreateApplication();
	app->Init();

	app->Run();

	delete app;
}

#endif
