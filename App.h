#pragma once
#include "Window.h"
#include <string>
#include "Drawable.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include <set>
#include <optional>
#include "Mesh.h"
#include "TestPlane.h"

class App
{
public:
	App(int Width = 800, int Height = 600, const std::string& title = "Direct3D");
	int Go();
private:
	void DoFrame();
	void ShowModelWindow();
private:
	ImguiManager m_ImGuiManager;
	Window m_Window;
	Camera m_Camera;
	Timer m_Timer;
	PointLight m_Light;
	struct
	{
		float pitch = 0.0f;
		float yaw = 0.0f;
		float roll = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	} m_Pos;
	int x = 0, y = 0;
	Model m_Wall{ m_Window.Gfx(),"Models\\brick_wall\\brick_wall.obj" };
	TestPlane m_TestPlane{ m_Window.Gfx(),1.0 };
};