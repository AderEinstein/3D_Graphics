#pragma once
#include "DrawableBase.h"

class Box : public DrawableBase<Box>
{
private:
	// speed (/dt)
	float droll;
	float dpitch;
	float dyaw;
	float dtheta;
	float dphi;
	float dchi;

	// position
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta;
	float phi;
	float chi;

	// model transform
	DirectX::XMFLOAT3X3 mt;

public:
	Box(Graphics& gfx, std::mt19937& rng,
		DirectX::XMFLOAT3 material,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist
	);
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
};