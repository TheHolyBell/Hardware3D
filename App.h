#pragma once
#include "Window.h"
#include <string>

class App
{
public:
	App(int Width = 800, int Height = 600, const std::string& title = "Direct3D");
	int Go();
private:
	void DoFrame();
private:
	Window m_Window;
};