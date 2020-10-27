#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"
#include "imgui/imgui.h"

class PointLight
{
private:
	struct PointLightCBuf
	{
		DirectX::XMFLOAT3 pos;
		float padding;
	};
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;
	mutable SolidSphere mesh;
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };;

public:
	PointLight(Graphics& gfx, float radius = 0.5);
	void Update(Graphics& gfx) const noexcept(!IS_DEBUG);
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
};

