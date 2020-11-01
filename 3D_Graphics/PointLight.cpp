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
		ImGui::Text("Intensity/Color");
		ImGui::ColorEdit3("Diffuse Color", &cbData.diffuseColor.x);
		ImGui::ColorEdit3("Ambient", &cbData.ambient.x);
		ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, 0.01f, 2.0f, "%.2f", 2);

		ImGui::Text("Position");
		ImGui::SliderFloat("X", &cbData.pos.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &cbData.pos.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &cbData.pos.z, -60.0f, 60.0f, "%.1f");

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &cbData.attConst, 0.05f, 10.0f, "%.2f", 4);
		ImGui::SliderFloat("Linear", &cbData.attLin, 0.0001f, 4.0f, "%.4f", 8);
		ImGui::SliderFloat("Quadratic", &cbData.attQuad, 0.0000001f, 10.0f, "%.7f", 10);

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Update(Window& wnd, DirectX::FXMMATRIX view) const noexcept(!IS_DEBUG)
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
		float cameraZ = 20.0f;
		cbData.pos.x = ((float)wnd.mouse.GetPosX() / (float(windowWidth) / 2) - 1.0f) * (cameraZ + cbData.pos.z);
		cbData.pos.y = (-(float)wnd.mouse.GetPosY() / (float(windowHeight) / 2) + 1.0f) * (cameraZ + cbData.pos.z) - (offsetY + cbData.pos.z * 0.33);
	}

	const auto dataCopy = cbData;
	const auto pos = DirectX::XMLoadFloat3(&cbData.pos);
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view)); // Make cbuf's dataCopy position relative to the camera

	cbuf.Update(wnd.Gfx(), dataCopy);			// Update light position constant buffer (relative to camera position)
	mesh.SetPos(cbData.pos);					// Update mesh/model transform (Note: the mesh's vertex transform const buffer is also made relative to the camera in the TransformCBuffer.cpp

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
		1.0f,
		0.027f,
		0.0028f,
		1.0f		 
	};
}