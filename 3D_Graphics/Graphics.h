#pragma once
#include <d3d11.h>
#include "AderWin.h"
#include "AderException.h"
#include "DxgiInfoManager.h"
#include <vector>


class Graphics
{
public:

	Graphics(HWND hWnd);
	~Graphics();
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
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

private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
};