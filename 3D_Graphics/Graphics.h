#pragma once
#include "AderWin.h"
#include <d3d11.h>
#include <wrl.h>
#include "AderException.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "DxgiInfoManager.h"
#include <memory>
#include <random>
#include <vector>


class Graphics
{

public:
	friend class Bindable;
	Graphics(HWND hWnd, int windowWidth, int windowHeight);
	~Graphics() = default;
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void DrawIndexed(UINT count) noexcept(!IS_DEBUG);
	void ClearBuffer(float red, float green, float blue) noexcept;
	void EndFrame();

	//***************************************** Graphics Exceptions ******************************************* 
	class Exception : public AderException
	{
		using AderException::AderException;
	};

	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* getType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};

	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* getType() const noexcept override;
	private:
		std::string reason;
	};

	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* getType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	//*********************************************************************************************************

private:
	DirectX::XMMATRIX projection;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
};