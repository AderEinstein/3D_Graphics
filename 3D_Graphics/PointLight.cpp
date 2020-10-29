#include "PointLight.h"

PointLight::PointLight(Graphics& gfx, int windowWidth, int windowHeight, float radius)
	:
	cbuf(gfx),
	mesh(gfx, radius),
	windowWidth(windowWidth),
	windowHeight(windowWidth)
{
	Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &cbData.pos.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &cbData.pos.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &cbData.pos.z, -60.0f, 60.0f, "%.1f");
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Update(Window& wnd) const noexcept(!IS_DEBUG)
{
	while (!wnd.mouse.IsEmpty())
	{
		const Mouse::Event e = (const Mouse::Event)wnd.mouse.Read();
		if (e.GetType() == Mouse::Event::Type::WheelUp)
		{
			cbData.pos.z += 1.0f;
		}
		else if (e.GetType() == Mouse::Event::Type::WheelDown)
		{
			cbData.pos.z -= 1.0f;
		}
	}
	if (wnd.mouse.LeftIsPressed()) 
	{
		float offsetY = 6.5f;	   // mouse offset to point light center
		float cameraR = 20.0f;
		cbData.pos.x = ((float)wnd.mouse.GetPosX() / (float(windowWidth) / 2) - 1.0f) * (cameraR + cbData.pos.z);
		cbData.pos.y = (-(float)wnd.mouse.GetPosY() / (float(windowHeight) / 2) + 1.0f) * (cameraR + cbData.pos.z) - (offsetY + cbData.pos.z * 0.33);
	}

	mesh.SetPos(cbData.pos);					// Update mesh/model transform
	cbuf.Update(wnd.Gfx(), cbData);				// Update light position constant buffer 
	cbuf.Bind(wnd.Gfx());						// Bind pixel shader constant buffer slot with light position constant bufer 
}

void PointLight::Draw(Graphics& gfx) const noexcept(!IS_DEBUG)
{
	mesh.Draw(gfx);
}

void PointLight::Reset() noexcept
{
	cbData = {
		{ 1.0f,1.0f,1.0f },
		{ 0.05f,0.05f,0.05f },
		{ 0.0f,0.0f,0.0f },
		{ 0.9f,0.5f,0.2f },
		1.0f,
		0.027f,
		0.0028f,
		1.0f		 
	};
}