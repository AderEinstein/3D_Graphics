#include "Window.h"


// Singleton:WindowClass Registration

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept 
	:
	hInst(GetModuleHandle(nullptr))
{
	// Register the window class.

	WNDCLASSEX wc = { };
	wc.lpfnWndProc = handleMsgSetup;
	wc.hInstance = getWndInstance();
	wc.lpszClassName = getWndClassName();
	wc.lpszMenuName = nullptr;
	wc.cbSize = sizeof(wc);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.style = CS_OWNDC;
	wc.hIcon = nullptr;
	wc.hIconSm = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;

	RegisterClassEx(&wc);
}

const wchar_t* Window::WindowClass::getWndClassName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::getWndInstance() noexcept
{
	return wndClass.hInst;
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, getWndInstance());
}


//*******************************************************************************************************************************************************

Window::Window(int width, int height, const wchar_t* WndName) noexcept
{
	// Calc Windows Rectangle Position
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	AdjustWindowRect(&wr, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, FALSE);

	hWnd = CreateWindow(
		WindowClass::getWndClassName(),				
		WndName,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,	
		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr,						// Parent Window
		nullptr,						// Menu
		WindowClass::getWndInstance(),	// Instance Handle
		this							// Additional Application Data
	);

	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

LRESULT CALLBACK Window::handleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// Use the "this" parameter that was passed in CreateWindow() to store window class pointer on WinAPI side
	if (msg == WM_NCCREATE)
	{
		// Extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// Set WinAPI-managed user data to store ptr to window class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// Set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::handleMsgLink));
		// Forward message to window class handler
		return pWnd->handleMsg(hWnd, msg, wParam, lParam);
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::handleMsgLink(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// Retrieve ptr to window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// Forward message to window class handler
	return pWnd->handleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::handleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	
	if (msg == WM_CLOSE)
	{
		PostQuitMessage(0);  
		return 0;  // Since we want to destroy the window using our window class destructor instead of using DefWindowProc, we return if we get a WM_CLOSE msg
	}

	return DefWindowProc(hWnd, msg, wParam, lParam); // The default window procedure will handle WM_QUIT msg
}