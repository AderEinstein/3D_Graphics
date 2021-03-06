#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"
#include "imgui/imgui.h"
#include "Window.h"

namespace dx = DirectX;

class PointLight
{
private:
	struct PointLightCBuf
	{	
		alignas(16) DirectX::XMFLOAT3 diffuseColor;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) mutable DirectX::XMFLOAT3 pos;
		float attConst;
		float attLin;
		float attQuad;
		float diffuseIntensity;
		float specularIntensity;
		float specularPower;
	}cbData;
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;
	mutable SolidSphere mesh;
	const int windowWidth;
	const int windowHeight;

public:
	PointLight(Graphics& gfx, int windowWidth, int windowHeight, float radius = 0.5);
	void Update(Window& wnd, DirectX::FXMMATRIX view) const noexcept(!IS_DEBUG);
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
};

