#pragma once
#include "AderWin.h"
#include "AderException.h"
#include "Graphics.h"
#include "keyboard.h"
#include "Mouse.h"
#include <optional>
#include <memory>

class Window
{
public:
	Window(int width, int height, const char* name);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window();
	void setTitle(const std::string& title);
	static std::optional<int> ProcessMessages();
	Graphics& Gfx();

	class Exception : public AderException
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* getType() const noexcept;
		static std::string translateErrorCode(HRESULT hr) noexcept;
		HRESULT getErrorCode() const noexcept;
		std::string getErrorString() const noexcept;
	private:
		HRESULT hr;
	};

	Keyboard kbd;
	Mouse mouse;

private:

	class WindowClass // Singleton which will manage window class registration
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

	static LRESULT CALLBACK handleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK handleMsgLink(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT handleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
};

