#include "App.h"
#include "Timer.h"
#include <iostream>

App::App(int Width, int Height, const std::string& title)
	: m_Window(Width, Height, title.c_str())
{
}

int App::Go()
{
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		DoFrame();
	}

	return msg.wParam;
}

void App::DoFrame()
{
	std::cout << "Here\n";
}
