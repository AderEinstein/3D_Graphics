#pragma once
#include "Window.h"
#include "WndExceptMacros.h"
#include "Timer.h"

class App
{
public:
	App();
	~App();
	int Go(); // Message loop

private:
	void DoFrame();
	Window wnd;
	std::vector<std::unique_ptr<class Box>> boxes;
	Timer timer1;
	Timer timer2;
};