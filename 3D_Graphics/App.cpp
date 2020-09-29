#include "App.h"

App::App()
	:
	wnd(1080, 720, "AEinstein3D Graphics")
{}

int App::Go()
{
	while (true)
	{
		if (const auto exit_code = Window::ProcessMessages())
		{
			return *exit_code;
		}
		DoFrame();
	}
	return 0;
}

void App::DoFrame()
{
	wnd.Gfx().EndFrame();
}