#pragma once
#include "DrawableBase.h"

namespace dx = DirectX;

class TexturedBox : public DrawableBase<TexturedBox>
{
public:
	TexturedBox( Graphics& gfx,std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist
		);
	void Update( float dt ) noexcept override;
	dx::XMMATRIX GetTransformXM() const noexcept override;
private:
	// model transform
	dx::XMFLOAT3X3 mt; // to scale the box vertices & obtain different box heights
	// positional
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta;
	float phi;
	float chi;
	// speed (delta/s)
	float droll;
	float dpitch;
	float dyaw;
	float dtheta;
	float dphi;
	float dchi;
};