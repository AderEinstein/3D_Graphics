#include "Window.h"
#include "WndExceptMacros.h"
#include <sstream>
#include "resource.h"

// Error exception helper macros
#define AWND_EXCEPT( hr ) Window::Exception( __LINE__,__FILE__,hr )
#define AWND_LAST_EXCEPT() Window::Exception( __LINE__,__FILE__,GetLastError() )

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
	wc.hIcon = static_cast<HICON>(LoadImage(getWndInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 64, 64, 0));
	wc.hIconSm = static_cast<HICON>(LoadImage(getWndInstance(), MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON, 32, 32, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;

	RegisterClassEx(&wc);
}

const char* Window::WindowClass::getWndClassName() noexcept
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


//********************************************************************************************************************************************************************

Window::Window(int width, int height, const char* WndName) 
{
	// Calc Windows Rectangle Position
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (FAILED(AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)))
	{
		throw WND_LAST_EXCEPT();
	};

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

	if (hWnd == nullptr)
	{
		throw WND_LAST_EXCEPT();
	}

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
	else if (msg == WM_KILLFOCUS) // Reset keyboard key_states when we switch window focus
	{
		kbd.ClearState();
	}
	// KEYBOARD MESSAGES
	else if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) // also handle syskey msgs to track ALT key(VK_MENU) and F10
	{
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled()) // Filter autorepeat
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
	}
	else if (msg == WM_KEYUP || msg == WM_SYSKEYUP)
	{
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
	}
	else if (msg == WM_CHAR)
	{
		kbd.OnChar(static_cast<unsigned char>(wParam));
	}

	return DefWindowProc(hWnd, msg, wParam, lParam); // The default window procedure will handle WM_QUIT msg
}

//************************************************************************************************************************************************************************
//Wnd Exceptions
Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
	:
	AderException(line, file),
	hr(hr)
{}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << getType() << std::endl
		<< "Error Code :: " << getErrorCode() << std::endl
		<< "Description :: " << getErrorString() << std::endl
		<< getSourceString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

std::string Window::Exception::translateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	DWORD msgLen = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	if (msgLen == 0)
	{
		return "Unidentified Error_Code";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::Exception::getErrorCode() const noexcept
{
	return hr;
}

const char* Window::Exception::getType() const noexcept
{
	return "Ader Window Exception";
}

std::string Window::Exception::getErrorString() const noexcept
{
	return translateErrorCode(hr);
}