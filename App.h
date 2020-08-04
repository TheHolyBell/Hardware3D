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
#include "ScriptCommander.h"

class App
{
public:
	App(int Width = 800, int Height = 600, const std::string& title = "Direct3D", const std::string& commandLine = "");
	int Go();
private:
	void DoFrame();
private:
	ImguiManager m_ImGuiManager;
	Window m_Window;
	Camera m_Camera;
	Timer m_Timer;
	PointLight m_Light;
	
	std::string m_CommandLine;
	ScriptCommander m_ScriptCommander;

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
	Model m_Gobber{ m_Window.Gfx(), "Models/gobber/GoblinX.obj", 6.0f };
	Model m_Wall{ m_Window.Gfx(), "Models/brick_wall/brick_wall.obj", 5.0f };
	Model m_Nano{ m_Window.Gfx(), "Models/nano_textured/nanosuit.obj", 4.0f};
	Model m_Sponza{ m_Window.Gfx(), "Models/Sponza/sponza.obj", 1.0f / 20.0f };
	TestPlane m_TestPlane = { m_Window.Gfx(), 6.0f, {0.3f, 0.3f, 1.0f, 0.0f} };
};