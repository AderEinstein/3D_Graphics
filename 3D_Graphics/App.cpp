#include "App.h"

GDIPlusManager gdipm;

App::App()
	:
	wnd(1080, 720, "AEinstein3D Graphics")
{
	Factory f(wnd.Gfx());
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

	wnd.Gfx().SetCamera(DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f));
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
	wnd.Gfx().BeginFrame(0, g, b);

	wnd.Gfx().SetCamera(cam.GetMatrix());

	float dt = timer2.Mark() * speed_factor;
	for (auto& d : drawables)
	{
		d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(wnd.Gfx());
	}
	
	// Imgui window to control simulation speed
	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 4.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Status: %s", wnd.kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING");
	}
	ImGui::End();

	// Imgui window to control camera
	cam.SpawnControlWindow();

	// Present frame buffer
	wnd.Gfx().EndFrame();
}

