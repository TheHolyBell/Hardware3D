#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "TestCube.h"
#include "Model.h"
#include "ScriptCommander.h"
#include "BlurOutlineRenderGraph.h"
#include "CameraContainer.h"

class App
{
public:
	App(int Width = 800, int Height = 600, const std::string& title = "Direct3D", const std::string& commandLine = "");
	int Go();
	~App();
private:
	void DoFrame(float dt);
	void HandleRawInput(float dt);
	void HandleGamepad(float dt);
	void HandleInput(float dt);
	void RenderControlSelectWindow(Graphics& gfx);

private:
	enum class ControlType
	{
		RawInput,
		Gamepad
	};
private:
	ImguiManager m_ImGuiManager;
	Window m_Window;
	CameraContainer m_CameraContainer;
	Timer m_Timer;
	PointLight m_Light;
	ControlType m_ControlType;

	int m_LeftMotorVibration = 0;
	int m_RightMotorVibration = 0;
	int m_Sensitivity = 0;

	ScriptCommander m_ScriptCommander;
	RenderGraph::BlurOutlineRenderGraph m_RenderGraph{m_Window.Gfx()};
	float m_Speedfactor = 1.0f;

	Model m_Sponza{ m_Window.Gfx(),"Models\\sponza\\sponza.obj",1.0f / 20.0f };
	Model m_Gobber{ m_Window.Gfx(), "Models\\gobber\\GoblinX.obj", 4.0f };
	Model m_Nano{ m_Window.Gfx(), "Models\\nano_textured\\nanosuit.obj", 2.0f };
};