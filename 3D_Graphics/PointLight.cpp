#include "PointLight.h"

PointLight::PointLight(Graphics& gfx, int windowWidth, int windowHeight, float radius)
	:
	mesh(gfx, radius),
	cbuf(gfx),
	windowWidth(windowWidth),
	windowHeight(windowWidth)
{}

void PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -60.0f, 60.0f, "%.1f");
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
			pos.z += 1.0f;
		}
		else if (e.GetType() == Mouse::Event::Type::WheelDown)
		{
			pos.z -= 1.0f;
		}
	}
	if (wnd.mouse.LeftIsPressed()) 
	{
		float offsetY = 6.5f;	   // mouse offset to point light center
		float cameraR = 20.0f;
		pos.x = ((float)wnd.mouse.GetPosX() / (float(windowWidth) / 2) - 1.0f) * (cameraR + pos.z);
		pos.y = (-(float)wnd.mouse.GetPosY() / (float(windowHeight) / 2) + 1.0f) * (cameraR + pos.z) - (offsetY + pos.z * 0.33);
	}

	mesh.SetPos(pos);							   // Update mesh/model transform
	cbuf.Update(wnd.Gfx(), PointLightCBuf{ pos }); // Update light position constant buffer 
	cbuf.Bind(wnd.Gfx());						   // Bind pixel shader constant buffer slot with light position constant bufer 
}

void PointLight::Draw(Graphics& gfx) const noexcept(!IS_DEBUG)
{
	mesh.Draw(gfx);
}

void PointLight::Reset() noexcept
{
	pos = { 0.0f,0.0f,0.0f };
}