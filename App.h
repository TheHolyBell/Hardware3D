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


class App
{
public:
	App(int Width = 800, int Height = 600, const std::string& title = "Direct3D", const std::string& commandLine = "");
	int Go();
	~App();
private:
	void DoFrame(float dt);
	void HandleInput(float dt);
private:
	ImguiManager m_ImGuiManager;
	Window m_Window;
	Camera m_Camera;
	Timer m_Timer;
	PointLight m_Light;

	ScriptCommander m_ScriptCommander;
	RenderGraph::BlurOutlineRenderGraph m_RenderGraph{m_Window.Gfx()};
	float m_Speedfactor = 1.0f;

	Model m_Sponza{ m_Window.Gfx(),"Models\\sponza\\sponza.obj",1.0f / 20.0f };
};