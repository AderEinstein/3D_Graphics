#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "AderWin.h"
#include "AderException.h"
#include "DxgiInfoManager.h"
#include <vector>


class Graphics
{
public:

	Graphics(HWND hWnd);
	~Graphics() = default;
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	void DrawTestTriangle(float angle, float x, float y);
	void ClearBuffer(float red, float green, float blue) noexcept;
	void EndFrame();

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

private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
};