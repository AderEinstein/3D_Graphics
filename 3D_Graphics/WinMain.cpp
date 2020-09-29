#include "Window.h"
#include "WndExceptMacros.h"
#include <sstream>

int CALLBACK WinMain(
	HINSTANCE hInstance,	
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	try
	{
		Window wnd(1080, 720, "AEinstein3D Graphics");

		MSG msg;
		BOOL exit_code;
		while ((exit_code = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			while (!wnd.mouse.IsEmpty())
			{
				const auto event = wnd.mouse.Read();
				switch (event.GetType())
				{
				case Mouse::Event::Type::Leave:
					wnd.setTitle("Mouse Away");
					break;
				case Mouse::Event::Type::Move:
				{
					std::ostringstream oss;
					oss << "Mouse moved to (" << event.GetPosX() << "," << event.GetPosY() << ")";
					wnd.setTitle(oss.str());
				}
				break;
				}
			}
		}
		//check if get message failed
		if (exit_code == -1)
		{
			WND_LAST_EXCEPT();
		}
	}
	catch (const AderException& e)
	{
		MessageBox(nullptr, e.what(), e.getType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;

}