#include "Window.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,	
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	Window wnd(1080, 720, L"AEinstein Win");

	MSG msg;
	BOOL exit_code;
	while ((exit_code = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (exit_code == -1)
	{
		return -1;
	}

	return msg.wParam; // This wparam is the value passed to PostQuitMessage
}