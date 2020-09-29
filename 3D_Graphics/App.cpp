#include "App.h"

App::App()
	:
	wnd(1080, 720, "AEinstein3D Graphics")
{}

int App::Go()
{
	Window wnd();

	MSG msg;
	BOOL exit_code;
	while ((exit_code = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (exit_code == -1)
	{
		WND_LAST_EXCEPT();
	}

	return msg.wParam;
}

void App::DoFrame()
{

}