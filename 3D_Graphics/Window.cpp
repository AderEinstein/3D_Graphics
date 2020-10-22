#include "Window.h"
#include "WndExceptMacros.h"
#include <sstream>
#include "imgui/imgui_impl_win32.h"
#include "resource.h"

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
	wc.hIconSm = static_cast<HICON>(LoadImage(getWndInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
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
	:
	width(width),
	height(height)
{
	// Calc Windows Rectangle Position
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if ((AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)) == 0)
	{
		throw WND_LAST_EXCEPT();
	}

	hWnd = CreateWindow(
		WindowClass::getWndClassName(),
		WndName,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		// Size and position
		200, 20, wr.right - wr.left, wr.bottom - wr.top,
		nullptr,						// Parent Window
		nullptr,						// Menu
		WindowClass::getWndInstance(),	// Instance Handle
		this							// Additional Application Data
	);

	if (hWnd == nullptr)
	{
		throw WND_LAST_EXCEPT();
	}

	// ImGui Win32 Impl Initialization
	ImGui_ImplWin32_Init(hWnd);

	// Show newly created window
	ShowWindow(hWnd, SW_SHOWDEFAULT);

	pGfx = std::make_unique<Graphics>(hWnd, width, height);
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}

LRESULT CALLBACK Window::handleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// Note: The "this" argument that was passed in CreateWindow() to store window class pointer on WinAPI side can be obtained from the lParam of incoming  messages
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
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true; // No need to check for the other window massages if the event came from an imgui window, so return immediately
	}
	else if (msg == WM_CLOSE)
	{
		PostQuitMessage(0);
		return 0;  // Since we want to destroy the window using our window class destructor instead of using DefWindowProc, we return if we get a WM_CLOSE msg
	}
	else if (msg == WM_KILLFOCUS) // Reset keyboard key_states when we switch window focus
	{
		kbd.ClearState();
	}
	///////////////////////////////////////////////////KEYBOARD MESSAGES////////////////////////////////////////////////////////////////
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

	/////////////////////////////////////////////////////MOUSE MESSAGES//////////////////////////////////////////////////////////////////
	else if (msg == WM_MOUSEMOVE)
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnMouseMove(pt.x, pt.y);
	}
	else if (msg == WM_LBUTTONDOWN)
	{
		const POINTS point = MAKEPOINTS(lParam);
		// In client region : log move, and log enter + capture mouse (if not previously in window)
		if (point.x >= 0 && point.x < width && point.y >= 0 && point.y < height)
		{
			mouse.OnMouseMove(point.x, point.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		// Out of client : log move, maintain capture if button down
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(point.x, point.y);
			}
			// Button up: release capture / log event for leaving
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
	}
	else if (msg == WM_RBUTTONDOWN)
	{
		const POINTS point = MAKEPOINTS(lParam);
		mouse.OnRightPressed(point.x, point.y);
	}
	else if (msg == WM_LBUTTONUP)
	{
		const POINTS point = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(point.x, point.y);

		// Release mouse when it goes outside of the window
		if (point.x < 0 || point.x >= width || point.y < 0 || point.y >= height)
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
	}
	else if (msg == WM_RBUTTONUP)
	{
		const POINTS point = MAKEPOINTS(lParam);
		mouse.OnRightReleased(point.x, point.y);
	}
	else if (msg == WM_MOUSEWHEEL)
	{
		const POINTS point = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(point.x, point.y, delta);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam); // The default window procedure will handle WM_QUIT msg
}

//************************************************************************************************************************************************************************
//Wnd Exceptions
std::string Window::Exception::translateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	const DWORD msgLen = FormatMessage(
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

Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << getType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< getSourceString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HrException::getType() const noexcept
{
	return "Window Exception";
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::translateErrorCode(hr);
}


const char* Window::NoGfxException::getType() const noexcept
{
	return "Window Exception [No Graphics]";
}

void Window::SetTitle(const std::string& title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw WND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))	// While queue has messages, remove and dispatch them (but do not block on empty queue)
	{
		// Check for quit because PeekMessage does not signal this via its return value
		if (msg.message == WM_QUIT)
		{
			return (int)msg.wParam;	// Return optional wrapping int to signal quit
		}

		TranslateMessage(&msg);	// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
		DispatchMessage(&msg);
	}

	// Return empty optional to not quit the app
	return {};
}

Graphics& Window::Gfx()
{
	if (!pGfx)
	{
		throw WND_NOGFX_EXCEPT();
	}
	return *pGfx;
}