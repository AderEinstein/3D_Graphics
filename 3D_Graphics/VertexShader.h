#pragma once
#include "Bindable.h"

class VertexShader : public Bindable
{
protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;

public:
	VertexShader(Graphics& gfx, const std::wstring& path);
	ID3DBlob* GetBytecode() const noexcept;
	void Bind(Graphics& gfx) noexcept override;
};