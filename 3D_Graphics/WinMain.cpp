#include <Windows.h>

int CALLBACK WinMain(
	HINSTANCE hInstance,	
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	// Register the window class.

	const wchar_t className[] = L"AEinstein3D";
	WNDCLASSEX wc = { };
	wc.lpfnWndProc = DefWindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = className;
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

	// Create a window instance.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		className,                      // Window class
		L"First Window baby",			// Window text
		WS_OVERLAPPEDWINDOW,            // Window style
		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr,       // Parent window    
		nullptr,       // Menu
		hInstance,	   // Instance handle
		nullptr        // Additional application data
	);

	if (hwnd == nullptr)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	// Message pump
	MSG msg;
	BOOL exit_code;
	while ( (exit_code = GetMessage( &msg, nullptr, 0, 0 )) > 0 )
	{
		TranslateMessage( &msg );
		DispatchMessage(&msg);
	}

	return 0;
}