#include "App.h"
#include "Timer.h"
#include <algorithm>
#include "Surface.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"

#include "RenderTarget.h"

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
#include "Gamepad.h"
#include "Channels.h"

#include "ShadowMappingPass.h"

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
	: m_Window(Width, Height, title.c_str()), m_Light(m_Window.Gfx()), m_ScriptCommander(TokenizeQuoted(commandLine)),
	m_ControlType(ControlType::RawInput)
{
	Graphics& gfx = m_Window.Gfx();

	m_Cube.SetPos({ 10.0f,5.0f,6.0f });
	m_Cube2.SetPos({ 10.0f,5.0f,14.0f });

	m_CameraContainer.AddCamera(std::make_unique<Camera>(gfx, "A", dx::XMFLOAT3{ -13.5f,6.0f,3.5f }, 0.0f, PI / 2.0f));
	m_CameraContainer.AddCamera(std::make_unique<Camera>(gfx, "B", dx::XMFLOAT3{ -13.5f,28.8f,-6.4f }, PI / 180.0f * 13.0f, PI / 180.0f * 61.0f));
	m_CameraContainer.AddCamera(m_Light.ShareCamera());

	m_Nano.SetRootTransform(
		dx::XMMatrixRotationY(PI / 2.f) *
		dx::XMMatrixTranslation(27.f, -0.56f, 1.7f)
	);
	m_Gobber.SetRootTransform(
		dx::XMMatrixRotationY(-PI / 2.f) *
		dx::XMMatrixTranslation(-8.f, 10.f, 0.f)
	);

	m_Light.LinkTechniques(m_RenderGraph);
	m_Sponza.LinkTechniques(m_RenderGraph);
	m_Gobber.LinkTechniques(m_RenderGraph);
	m_Nano.LinkTechniques(m_RenderGraph);
	m_Cube.LinkTechniques(m_RenderGraph);
	m_Cube2.LinkTechniques(m_RenderGraph);
	m_CameraContainer.LinkTechniques(m_RenderGraph);

	m_RenderGraph.BindShadowCamera(*m_Light.ShareCamera());
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
	m_Light.Bind(gfx, m_CameraContainer->GetMatrix());
	m_RenderGraph.BindMainCamera(m_CameraContainer.GetActiveCamera());

	m_CameraContainer.Submit(Channels::main);
	m_Light.Submit(Channels::main);
	m_Cube.Submit(Channels::main);
	m_Cube2.Submit(Channels::main);
	m_Sponza.Submit(Channels::main);
	m_Gobber.Submit(Channels::main);
	m_Nano.Submit(Channels::main);


	//m_Sponza.Submit(Channels::shadow);
	m_Cube.Submit(Channels::shadow);
	m_Cube2.Submit(Channels::shadow);
	m_Gobber.Submit(Channels::shadow);
	m_Nano.Submit(Channels::shadow);
	m_Sponza.Submit(Channels::shadow);

	m_RenderGraph.Execute(gfx);

	if (m_bSavingShadow)
	{
		m_RenderGraph.DumpShadowMap(gfx, "shadow.png");
		m_bSavingShadow = false;
	}

	if (m_bSavingDepth)
	{
		m_RenderGraph.StoreDepth(gfx, "depth.png");
		m_bSavingDepth = false;
	}

	static MP _ModelProbe;

	_ModelProbe.SpawnWindow(m_Sponza);
	_ModelProbe.SpawnWindow(m_Gobber);
	_ModelProbe.SpawnWindow(m_Nano);
	
	
	m_CameraContainer.SpawnWindow(gfx);
	m_Light.SpawnControlWindow(gfx);
	m_RenderGraph.RenderWidgets(gfx);

	m_Cube.SpawnControlWindow(gfx, "Cube 1");
	m_Cube2.SpawnControlWindow(gfx, "Cube 2");

	RenderControlSelectWindow(gfx);

	gfx.EndFrame();

	m_RenderGraph.Reset();
}

void App::HandleRawInput(float dt)
{
	if (!m_Window.CursorEnabled())
	{
		if (m_Window.g_Keyboard.KeyIsPressed('W'))
		{
			m_CameraContainer->Translate({ 0.0f,0.0f,dt });
		}
		if (m_Window.g_Keyboard.KeyIsPressed('A'))
		{
			m_CameraContainer->Translate({ -dt,0.0f,0.0f });
		}
		if (m_Window.g_Keyboard.KeyIsPressed('S'))
		{
			m_CameraContainer->Translate({ 0.0f,0.0f,-dt });
		}
		if (m_Window.g_Keyboard.KeyIsPressed('D'))
		{
			m_CameraContainer->Translate({ dt,0.0f,0.0f });
		}
		if (m_Window.g_Keyboard.KeyIsPressed(VK_SPACE))
		{
			m_CameraContainer->Translate({ 0.0f,dt,0.0f });
		}
		if (m_Window.g_Keyboard.KeyIsPressed(VK_SHIFT))
		{
			m_CameraContainer->Translate({ 0.0f,-dt,0.0f });
		}
	}

	while (const auto delta = m_Window.g_Mouse.ReadRawDelta())
	{
		if (!m_Window.CursorEnabled())
		{
			std::cout << "DeltaX: " << delta->x << " " << "DeltaY: " << delta->y << std::endl;
			m_CameraContainer->Rotate((float)delta->x, (float)delta->y);
		}
	}
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
		case VK_DELETE:
			m_bSavingDepth = true;
			break;
		case VK_INSERT:
			m_bSavingShadow = true;
			break;
		}
	}

	switch (m_ControlType)
	{
	case ControlType::Gamepad:
		HandleGamepad(dt);
		break;
	case ControlType::RawInput:
		HandleRawInput(dt);
		break;
	}
}

void App::HandleGamepad(float dt)
{
	if (!m_Window.CursorEnabled())
	{
		Gamepad& _Gamepad = Gamepad::Get();
		_Gamepad.UpdateState();
		auto Left = _Gamepad.LeftStick();
		auto Right = _Gamepad.RightStick();
		m_Sensitivity = _Gamepad.GetSensitivity();

		m_CameraContainer->Rotate(Right.first * m_Sensitivity * dt, -Right.second * m_Sensitivity * dt);

		std::cout << "DeltaX: " << Right.first  << " " << "DeltaY: " << Right.second  << std::endl;

		m_CameraContainer->Translate({ Left.first * dt, 0.0f, Left.second * dt });

		if (_Gamepad.IsPressed(Button::GAMEPAD_A) || _Gamepad.IsPressed(Button::GAMEPAD_ARROW_UP))
			m_CameraContainer->Translate({ 0.0f, dt, 0.0f });
		if (_Gamepad.IsPressed(Button::GAMEPAD_B) || _Gamepad.IsPressed(Button::GAMEPAD_ARROW_DOWN))
			m_CameraContainer->Translate({ 0.0f, -dt, 0.0f });
	}
}

void App::RenderControlSelectWindow(Graphics& gfx)
{
	static const char* _ControlTypes[] = { "RawInput", "Gamepad" };
	static const char* _CurrentItem = _ControlTypes[0];
	if (ImGui::Begin("Control Type"))
	{
		if (ImGui::BeginCombo("Type", _CurrentItem))
		{
			for (int i = 0; i < std::size(_ControlTypes); ++i)
			{
				const bool _bSelected = _CurrentItem == _ControlTypes[i];
				if (ImGui::Selectable(_ControlTypes[i], _bSelected))
				{
					_CurrentItem = _ControlTypes[i];
					if (_CurrentItem == _ControlTypes[0])
						m_ControlType = ControlType::RawInput;
					else if (_CurrentItem == _ControlTypes[1])
						m_ControlType = ControlType::Gamepad;
				}
				if (_bSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (m_ControlType == ControlType::Gamepad)
		{
			ImGui::SliderInt("Left motor vibration percent: ", &m_LeftMotorVibration, 0, 100);
			ImGui::SliderInt("Right motor vibration percent: ", &m_RightMotorVibration, 0, 100);
			if (ImGui::Button("Apply"))
				Gamepad::Get().SetVibration(m_LeftMotorVibration, m_RightMotorVibration);
			if (ImGui::SliderInt("Sensitivity", &m_Sensitivity, 1000, 2000))
				Gamepad::Get().SetSensitivity(m_Sensitivity);
		}
	}

	ImGui::Image((ImTextureID)dynamic_cast<RenderGraph::ShadowMappingPass&>(m_RenderGraph.FindPassByName("shadowMap")).GetUnderlyingHandle(),
		ImVec2{ 200, 200 });

	ImGui::End();
}
