#pragma once
#include "Window.h"
#include "WndExceptMacros.h"
#include "Timer.h"
#include "Box.h"
#include "Pyramid.h"
#include "AssimpTest.h"
#include "AderMath.h"
#include <Algorithm>
#include <memory>
#include "GDIPlusManager.h"
#include "Surface.h"
#include "Sheet.h"
#include "TexturedBox.h"
#include "ImguiManager.h"
#include "imgui/imgui.h"
#include "PointLight.h"
#include "Camera.h"
#include "Model.h"

class App
{
public:
	App();
	~App();
	int Go(); // Message loop

private:
	void DoFrame();
	void ShowModelWindow();

	ImguiManager imgui;
	Window wnd;
	PointLight light;
	Camera cam;
	Timer timer1;
	Timer timer2;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 50;
	bool annimateBackground = false;
	float speed_factor = 1.0f;
	Model nano{ wnd.Gfx(), "Models\\nanosuit.obj" };
	struct
	{
		float roll = 0.0f;
		float pitch = PI;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = -8.0f;
		float z = 0.0f;
	} modelPos;

	class Factory
	{
	public:
		Factory(Graphics& gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			const DirectX::XMFLOAT3 material = { cdist(rng), cdist(rng), cdist(rng) };
			switch (typedist(rng))
			{/*
			case 0:
				return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			case 1:
				return std::make_unique<Sheet>(
					gfx, rng, adist, ddist,
					odist, rdist
					);*/
			case 2:
				return std::make_unique<TexturedBox>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
					);
			case 3:
				return std::make_unique<Box>(
					gfx, rng, material, adist, ddist,
					odist, rdist, bdist
					);
			case 4:
				return std::make_unique<AssimpTest>(
					gfx, rng, adist, ddist,
					odist, rdist, 1.5f, material
					);
			default:
				assert(false && "Bad drawable type in factory");
				return {};
			}
		}
	private:
		Graphics& gfx;
		GDIPlusManager gdipm;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> cdist{ 0.0f, 1.0f };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> typedist{ 2, 4};
	};
};