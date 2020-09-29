#pragma once
#include "Window.h"
#include "WndExceptMacros.h"

class App
{
public:
	App();
	int Go(); // Message loop

private:
	void DoFrame();
	Window wnd;
};