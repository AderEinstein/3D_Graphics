#pragma once
#include "Bindable.h"

class IndexBuffer : public Bindable
{
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	UINT count;

public:
	IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices);
	UINT GetCount() const noexcept;
	void Bind(Graphics& gfx) noexcept override;
};