#pragma once
#include "ConstantBuffers.h"
#include <DirectXMath.h>
#include "Drawable.h"

class TransformCBuffer : public Bindable
{
private:
	std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> pVcbuf;
	const Drawable& parent;

public:
	TransformCBuffer(Graphics& gfx, const Drawable& parent);
	void Bind(Graphics& gfx) noexcept override;
};