#pragma once
#include "AderWin.h"

class Window
{
public:
	Window(int width, int height, const wchar_t* name) noexcept;
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window();
private:
	class WindowClass // Sincleton which will manage window class registration
	{
	public:
		static const wchar_t* getName() noexcept;
		static HINSTANCE getInstance() noexcept;
	private:
		WindowClass() noexcept;
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		~WindowClass();
		static constexpr const wchar_t* wndClassName = L"AderEinstein 3D Graphics Engine";
		HINSTANCE hInst;
		static WindowClass wndClass;
	};

	static LRESULT CALLBACK handleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK handleMsgLink(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT handleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	int width;
	int height;
	HWND hWnd;
};

