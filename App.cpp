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
	: m_Window(Width, Height, title.c_str()), m_Light(m_Window.Gfx()),
	m_CommandLine(commandLine), m_ScriptCommander(TokenizeQuoted(commandLine)),
	m_FrameCommander(m_Window.Gfx())
{
	//TestDynamicConstant();
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
	static float speed_factor = 1.0f;
	const auto dt = m_Timer.Mark() * speed_factor;
	Graphics& gfx = m_Window.Gfx();
	gfx.BeginFrame(0.07f, 0.0f, 0.12f);
	gfx.SetCamera(m_Camera.GetMatrix());
	m_Light.Bind(gfx, m_Camera.GetMatrix());

	//wall.Draw( wnd.Gfx() );
	//tp.Draw( wnd.Gfx() );
	//nano.Draw( wnd.Gfx() );
	//gobber.Draw( wnd.Gfx() );
	m_Light.Submit(m_FrameCommander);
	m_Sponza.Submit(m_FrameCommander);
	m_Cube.Submit(m_FrameCommander);
	m_Cube2.Submit(m_FrameCommander);
	//bluePlane.Draw( wnd.Gfx() );
	//redPlane.Draw( wnd.Gfx() );
	m_FrameCommander.Execute(gfx);

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
		if (m_Window.g_Keyboard.KeyIsPressed('R'))
		{
			m_Camera.Translate({ 0.0f,dt,0.0f });
		}
		if (m_Window.g_Keyboard.KeyIsPressed('F'))
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

	// Mesh techniques window
	class TP : public TechniqueProbe
	{
	public:
		void OnSetTechnique() override
		{
			using namespace std::string_literals;
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, m_pTech->GetName().c_str());
			bool active = m_pTech->IsActive();
			ImGui::Checkbox(("Tech Active##"s + std::to_string(techIdx)).c_str(), &active);
			m_pTech->SetActiveState(active);
		}
		bool OnVisitBuffer(Dynamic::Buffer& buf) override
		{
			namespace dx = DirectX;
			float dirty = false;
			const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
			auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(bufIdx)]
			(const char* label) mutable
			{
				tagScratch = label + tagString;
				return tagScratch.c_str();
			};

			if (auto v = buf["scale"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("Scale"), &v, 1.0f, 2.0f, "%.3f", 3.5f));
			}
			if (auto v = buf["offset"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("offset"), &v, 0.0f, 1.0f, "%.3f", 2.5f));
			}
			if (auto v = buf["materialColor"]; v.Exists())
			{
				dcheck(ImGui::ColorPicker3(tag("Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
			}
			if (auto v = buf["specularColor"]; v.Exists())
			{
				dcheck(ImGui::ColorPicker3(tag("Spec. Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
			}
			if (auto v = buf["specularGloss"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f", 1.5f));
			}
			if (auto v = buf["specularWeight"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("Spec. Weight"), &v, 0.0f, 2.0f));
			}
			if (auto v = buf["useSpecularMap"]; v.Exists())
			{
				dcheck(ImGui::Checkbox(tag("Spec. Map Enable"), &v));
			}
			if (auto v = buf["useNormalMap"]; v.Exists())
			{
				dcheck(ImGui::Checkbox(tag("Normal Map Enable"), &v));
			}
			if (auto v = buf["normalMapWeight"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("Normal Map Weight"), &v, 0.0f, 2.0f));
			}
			return dirty;
		}
	};

	class MP : ModelProbe
	{
	public:
		void SpawnWindow(Model& model)
		{
			ImGui::Begin("Model");
			ImGui::Columns(2, nullptr, true);
			model.Accept(*this);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr)
			{
				bool dirty = false;
				const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
				auto& tf = ResolveTransform();
				ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Translation");
				dcheck(ImGui::SliderFloat("X", &tf.x, -60.f, 60.f));
				dcheck(ImGui::SliderFloat("Y", &tf.y, -60.f, 60.f));
				dcheck(ImGui::SliderFloat("Z", &tf.z, -60.f, 60.f));
				ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Orientation");
				dcheck(ImGui::SliderAngle("X-rotation", &tf.xRot, -180.0f, 180.0f));
				dcheck(ImGui::SliderAngle("Y-rotation", &tf.yRot, -180.0f, 180.0f));
				dcheck(ImGui::SliderAngle("Z-rotation", &tf.zRot, -180.0f, 180.0f));
				if (dirty)
				{
					pSelectedNode->SetAppliedTransform(
						dx::XMMatrixRotationX(tf.xRot) *
						dx::XMMatrixRotationY(tf.yRot) *
						dx::XMMatrixRotationZ(tf.zRot) *
						dx::XMMatrixTranslation(tf.x, tf.y, tf.z)
					);
				}
			}
			if (pSelectedNode != nullptr)
			{
				TP probe;
				pSelectedNode->Accept(probe);
			}
			ImGui::End();
		}
	protected:
		bool PushNode(Node& node) override
		{
			// if there is no selected node, set selectedId to an impossible value
			const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
			// build up flags for current node
			const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
				| ((node.GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
				| (node.HasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf);
			// render this node
			const auto expanded = ImGui::TreeNodeEx(
				(void*)(intptr_t)node.GetId(),
				node_flags, node.GetName().c_str()
			);
			// processing for selecting node
			if (ImGui::IsItemClicked())
			{
				// used to change the highlighted node on selection change
				struct Probe : public TechniqueProbe
				{
					virtual void OnSetTechnique()
					{
						if (m_pTech->GetName() == "Outline")
						{
							m_pTech->SetActiveState(highlighted);
						}
					}
					bool highlighted = false;
				} probe;

				// remove highlight on prev-selected node
				if (pSelectedNode != nullptr)
				{
					pSelectedNode->Accept(probe);
				}
				// add highlight to newly-selected node
				probe.highlighted = true;
				if (probe.highlighted && pSelectedNode != nullptr)
					std::cout << "Highlited: " << pSelectedNode->GetId() << std::endl;
				node.Accept(probe);

				pSelectedNode = &node;
			}
			// signal if children should also be recursed
			return expanded;
		}
		void PopNode(Node& node) override
		{
			ImGui::TreePop();
		}
	private:
		Node* pSelectedNode = nullptr;
		struct TransformParameters
		{
			float xRot = 0.0f;
			float yRot = 0.0f;
			float zRot = 0.0f;
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
		};
		std::unordered_map<int, TransformParameters> transformParams;
	private:
		TransformParameters& ResolveTransform() noexcept
		{
			const auto id = pSelectedNode->GetId();
			auto i = transformParams.find(id);
			if (i == transformParams.end())
			{
				return LoadTransform(id);
			}
			return i->second;
		}
		TransformParameters& LoadTransform(int id) noexcept
		{
			const auto& applied = pSelectedNode->GetAppliedTransform();
			const auto angles = ExtractEulerAngles(applied);
			const auto translation = ExtractTranslation(applied);
			TransformParameters tp;
			tp.zRot = angles.z;
			tp.xRot = angles.x;
			tp.yRot = angles.y;
			tp.x = translation.x;
			tp.y = translation.y;
			tp.z = translation.z;
			return transformParams.insert({ id,{ tp } }).first->second;
		}
	};
	static MP modelProbe;

	// imgui windows
	modelProbe.SpawnWindow(m_Sponza);
	m_Camera.SpawnControlWindow();
	m_Light.SpawnControlWindow(gfx);
	m_Cube.SpawnControlWindow(gfx, "Cube 1");
	m_Cube2.SpawnControlWindow(gfx, "Cube 2");
	m_FrameCommander.ShowWindows(gfx);
	//sponza.ShowWindow( wnd.Gfx(),"Sponza" );
	//gobber.ShowWindow( wnd.Gfx(),"gobber" );
	//wall.ShowWindow( wnd.Gfx(),"Wall" );
	//tp.SpawnControlWindow( wnd.Gfx() );
	//nano.ShowWindow( wnd.Gfx(),"Nano" );
	//bluePlane.SpawnControlWindow( wnd.Gfx(),"Blue Plane" );
	//redPlane.SpawnControlWindow( wnd.Gfx(),"Red Plane" );

	// present
	gfx.EndFrame();
	m_FrameCommander.Reset();
}
