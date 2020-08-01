#include "App.h"
#include "Timer.h"
#include <algorithm>
#include "Surface.h"
#include "GDIPlusManager.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"

#include "Vertex.h"
#include <iostream>
#include "BindableCommon.h"
#include "ShaderReflector.h"
#include "BindableCommon.h"

GDIPlusManager g_GDIPlusManager;

#define PI 3.14159

namespace dx = DirectX;


App::App(int Width, int Height, const std::string& title)
	: m_Window(Width, Height, title.c_str()), m_Light(m_Window.Gfx())
{
	m_Wall.SetRootTransform(dx::XMMatrixTranslation(-1.5f, 0.0f, 0.0f));
	m_TestPlane.SetPos({ 1.5f, 0.0f, 0.0f });
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
		else
			DoFrame();
	}

	return msg.wParam;
}

void App::DoFrame()
{
	static float _speedFactor = 1.0f;
	Graphics& gfx = m_Window.Gfx();

	auto dt = m_Timer.Mark() * _speedFactor;


	gfx.BeginFrame(0.07f, 0.0f, 0.12f);
	gfx.SetCamera(m_Camera.GetMatrix());
	m_Light.Bind(gfx, gfx.GetCamera());

	m_Wall.Draw(gfx);
	m_TestPlane.Draw(gfx);
	m_Light.Draw(gfx);

	while (const auto e = m_Window.g_Keyboard.ReadKey())
	{
		if (!e->IsPress())
			continue;

		switch (e->GetCode())
		{
		case VK_ESCAPE:
			if (m_Window.CursorEnabled())
			{
				m_Window.DisableCursor();
				m_Window.g_Mouse.EnableRaw();
			}
			else
			{
				m_Window.EnableCursor();
				m_Window.g_Mouse.DisableRaw();
			}
			break;
		}
	}

	if (!m_Window.CursorEnabled())
	{
		if (m_Window.g_Keyboard.KeyIsPressed('W'))
			m_Camera.Translate({ 0.0f, 0.0f, dt });
		if (m_Window.g_Keyboard.KeyIsPressed('A'))
			m_Camera.Translate({ -dt,0.0f,0.0f });;
		if (m_Window.g_Keyboard.KeyIsPressed('S'))
			m_Camera.Translate({ 0.0f,0.0f,-dt });
		if (m_Window.g_Keyboard.KeyIsPressed('D'))
			m_Camera.Translate({ dt,0.0f,0.0f });
		if (m_Window.g_Keyboard.KeyIsPressed(VK_SPACE))
			m_Camera.Translate({ 0.0f,dt,0.0f });
		if (m_Window.g_Keyboard.KeyIsPressed(VK_SHIFT))
			m_Camera.Translate({ 0.0f,-dt,0.0f });
	}

	while (const auto delta = m_Window.g_Mouse.ReadRawDelta())
	{
		if (!m_Window.CursorEnabled())
			m_Camera.Rotate(delta->x, delta->y);
	}

	ShowModelWindow();
	m_TestPlane.SpawnControlWindow(gfx);
	m_Camera.SpawnControlWindow();
	m_Light.SpawnControlWindow();

	gfx.EndFrame();
}

void App::ShowModelWindow()
{
	m_Wall.ShowWindow();
}
