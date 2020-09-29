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
	const float c = cos(timer.Peek()) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer(0, c, 1.0f);
	wnd.Gfx().DrawTestTriangle();
	wnd.Gfx().EndFrame();
}