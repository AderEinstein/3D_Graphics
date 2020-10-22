#include "App.h"

GDIPlusManager gdipm;

App::App()
	:
	wnd(800, 600, "AEinstein3D Graphics")
{
	Factory f(wnd.Gfx());
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

	const auto s = Surface::FromFile("Images//Kakashi reads.jpg");

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 0.75f, 0.5f, 40.0f));
}

App::~App()
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
	const float g = cos(timer1.Peek()) / 2.0f + 0.33f;
	const float b = std::max( 0.13f, cos(timer1.Peek()) / 2.0f + 0.5f);
	wnd.Gfx().ClearBuffer(0, g, b);

	float dt = timer2.Mark();
	for (auto& d : drawables)
	{
		d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(wnd.Gfx());
	}
	wnd.Gfx().EndFrame();
}

