#pragma once
#include "AderWin.h"
#include "AderException.h"
#include "Graphics.h"
#include "keyboard.h"
#include "Mouse.h"
#include <optional>
#include <memory>
#include "PointLight.h"

class Window
{
public:
	Window(int width, int height, const char* name);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window();
	
	void SetTitle(const std::string& title);
	static std::optional<int> ProcessMessages() noexcept;
	Graphics& Gfx();

	Keyboard kbd;
	Mouse mouse;

	//***************************************** Window Exceptions ********************************************* 
	class Exception : public AderException
	{
		using AderException::AderException;
	public:
		static std::string translateErrorCode(HRESULT hr) noexcept;
	};

	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* getType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};

	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* getType() const noexcept override;
	};
	// ************************************************************************************************

private:

	// ********************** Singleton which manages window class registration ***********************
	class WindowClass 
	{
	public:
		static const char* getWndClassName() noexcept;
		static HINSTANCE getWndInstance() noexcept;
	private:
		WindowClass() noexcept;
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		~WindowClass();
		static constexpr const char* wndClassName = "AEinstein 3D";
		HINSTANCE hInst;
		static WindowClass wndClass;
	};
	//***********************************************************************************************

	static LRESULT CALLBACK handleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK handleMsgLink(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT handleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
	int width;
	int height;
	
};

