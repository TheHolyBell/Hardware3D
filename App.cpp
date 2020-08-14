#include "App.h"
#include "Timer.h"
#include <algorithm>
#include "Surface.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"

#include "ChiliUtil.h"
#include "Vertex.h"
#include <iostream>
#include "BindableCommon.h"
#include "ShaderReflector.h"
#include "BindableCommon.h"
#include "DynamicConstant.h"
#include "LayoutCodex.h"

#include "ModelProbe.h"
#include "TestModelProbe.h"
#include "Node.h"
#include "ChiliXM.h"

#define PI 3.14159

namespace dx = DirectX;


void TestDynamicConstant()
{
	using namespace std::string_literals;

	{
		Dynamic::RawLayout layout;
		layout.Add<Dynamic::Float3>("floaty");
		layout.Add<Dynamic::Float2>("flo2y");
		layout.Add<Dynamic::Struct>("structy");
		layout["structy"].Add<Dynamic::Float>("floaty");
		layout.Add<Dynamic::Array>("arr");
		layout["arr"].Set<Dynamic::Float>(10);

		auto buf = Dynamic::Buffer(std::move(layout));
		buf["structy"]["floaty"s] = 420.0f;
		buf["floaty"] = DirectX::XMFLOAT3{ 1337, 1488, 69 };
		buf["flo2y"] = DirectX::XMFLOAT2{ 69, 69 };
		buf["arr"][7] = 13371488.0f;

		float f2 = buf["structy"]["floaty"s];
		float f3 = buf["arr"][7];

		std::cout << "structy.floaty = " << f2 << std::endl;
		std::cout << std::fixed << "arr[7] = " << f3 << std::endl;
		DirectX::XMFLOAT3 val1 = buf["floaty"];
		DirectX::XMFLOAT2 val2 = buf["flo2y"];

		std::cout << "floaty: " << val1.x << " " << val1.y << " " << val1.z << std::endl;
		std::cout << "flo2y: " << val2.x << " " << val2.y << std::endl;
	}

	{
		/*Dynamic::RawLayout s;
		s.Add<Dynamic::Struct>("Butts");
		s["Butts"].Add<Dynamic::Float3>("Pubes");
		s["Butts"].Add<Dynamic::Float>("Danks");

		s.Add<Dynamic::Float>("Woot");
		s.Add<Dynamic::Array>("Arr");

		s["Arr"].Set<Dynamic::Struct>(4);
		s["Arr"].T().Add<Dynamic::Float3>("Twerk");
		s["Arr"].T().Add<Dynamic::Array>("Werk");
		s["Arr"].T()["Werk"].Set<Dynamic::Float>(6);
		s["Arr"].T().Add<Dynamic::Array>("Meta");
		s["Arr"].T()["Meta"].Set<Dynamic::Array>(6);
		s["Arr"].T()["Meta"].T().Set<Dynamic::Matrix>(4);
		s["Arr"].T().Add<Dynamic::Bool>("Booler");
		*/
	}
}

App::App(int Width, int Height, const std::string& title, const std::string& commandLine)
	: m_Window(Width, Height, title.c_str()), m_Light(m_Window.Gfx()), m_ScriptCommander(TokenizeQuoted(commandLine))
{
	m_Light.LinkTechniques(m_RenderGraph);
	m_Sponza.LinkTechniques(m_RenderGraph);
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
		{
			const auto dt = m_Timer.Mark() * m_Speedfactor;
			HandleInput(dt);
			DoFrame(dt);
		}
	}

	return msg.wParam;
}

App::~App()
{
}

void App::DoFrame(float dt)
{
	Graphics& gfx = m_Window.Gfx();
	gfx.BeginFrame(0.07f, 0.0f, 0.12f);
	gfx.SetCamera(m_Camera.GetMatrix());
	m_Light.Bind(gfx, m_Camera.GetMatrix());

	m_Light.Submit();
	m_Sponza.Submit();

	m_RenderGraph.Execute(gfx);

	static MP _ModelProbe;

	_ModelProbe.SpawnWindow(m_Sponza);
	m_Camera.SpawnControlWindow();
	m_Light.SpawnControlWindow(gfx);

	gfx.EndFrame();
	m_RenderGraph.Reset();
}

void App::HandleInput(float dt)
{
	while (const auto e = m_Window.g_Keyboard.ReadKey())
	{
		if (!e->IsPress())
		{
			continue;
		}

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
		{
			m_Camera.Translate({ 0.0f,0.0f,dt });
		}
		if (m_Window.g_Keyboard.KeyIsPressed('A'))
		{
			m_Camera.Translate({ -dt,0.0f,0.0f });
		}
		if (m_Window.g_Keyboard.KeyIsPressed('S'))
		{
			m_Camera.Translate({ 0.0f,0.0f,-dt });
		}
		if (m_Window.g_Keyboard.KeyIsPressed('D'))
		{
			m_Camera.Translate({ dt,0.0f,0.0f });
		}
		if (m_Window.g_Keyboard.KeyIsPressed(VK_SPACE))
		{
			m_Camera.Translate({ 0.0f,dt,0.0f });
		}
		if (m_Window.g_Keyboard.KeyIsPressed(VK_SHIFT))
		{
			m_Camera.Translate({ 0.0f,-dt,0.0f });
		}
	}

	while (const auto delta = m_Window.g_Mouse.ReadRawDelta())
	{
		if (!m_Window.CursorEnabled())
		{
			m_Camera.Rotate((float)delta->x, (float)delta->y);
		}
	}
}
