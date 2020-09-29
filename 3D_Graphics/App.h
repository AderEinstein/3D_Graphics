#pragma once
#include "Window.h"
#include "WndExceptMacros.h"
#include "Timer.h"

class App
{
public:
	App();
	int Go(); // Message loop

private:
	void DoFrame();
	Window wnd;
	Timer timer;
};