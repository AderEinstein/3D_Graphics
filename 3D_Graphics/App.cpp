#include "App.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

constexpr int WINDOW_WIDTH = 1080;
constexpr int WINDOW_HEIGHT = 720;

App::App()
	:
	wnd( WINDOW_WIDTH, WINDOW_HEIGHT, "AEinstein3D Graphics" ),
	light(wnd.Gfx(), WINDOW_WIDTH, WINDOW_HEIGHT)
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
	//Sky blue background animation colors
	const float g = (annimateBackground)? cos(0.5f*timer1.Peek()) / 2.0f + 0.35f : 0.05f;
	const float b = (annimateBackground)? cos(0.5f*timer1.Peek()) / 2.0f + 0.5f : 0.09f;
	
	wnd.Gfx().BeginFrame(0, g, b);

	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Update(wnd, cam.GetMatrix());

	float dt = timer2.Mark() * speed_factor;
	for (auto& d : drawables)
	{
		d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(wnd.Gfx());
	}

	const auto transform = 
		dx::XMMatrixRotationRollPitchYaw(modelPos.roll, modelPos.pitch, modelPos.yaw) *
		dx::XMMatrixTranslation(modelPos.x, modelPos.y, modelPos.z);
	nano.Draw(wnd.Gfx(), transform);
	light.Draw(wnd.Gfx());
	
	// Imgui window to control simulation speed
	if (ImGui::Begin("Simulation"))
	{
		ImGui::Checkbox("Background Color Animation", &annimateBackground);
		ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 4.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Status: %s", wnd.kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING");
	}
	ImGui::End();

	// Imgui windows to control camera and light
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowModelWindow();

	// Present frame buffer
	wnd.Gfx().EndFrame();
}

void App::ShowModelWindow()
{
	if (ImGui::Begin("Model"))
	{
		using namespace std::string_literals;

		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &modelPos.roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &modelPos.pitch, 0.0f, 360.0f);
		ImGui::SliderAngle("Yaw", &modelPos.yaw, -180.0f, 180.0f);

		ImGui::Text("Position");
		ImGui::SliderFloat("X", &modelPos.x, -20.0f, 20.0f);
		ImGui::SliderFloat("Y", &modelPos.y, -20.0f, 20.0f);
		ImGui::SliderFloat("Z", &modelPos.z, -20.0f, 20.0f);
	}
	ImGui::End();
}
