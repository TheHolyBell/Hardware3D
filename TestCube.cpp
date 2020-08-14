#include "TestCube.h"
#include "Cube.h"
#include "BindableCommon.h"
#include "ConstantBuffersEx.h"
#include "TransformCbufDoubleboi.h"
#include "ImGui\imgui.h"
#include "ImGui\ImGuizmo.h"
#include "Stencil.h"
#include "DynamicConstant.h"
#include "ShaderReflector.h"

TestCube::TestCube(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Cube::MakeIndependentTextured();
	model.Transform(dx::XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();
	const auto geometryTag = "$cube." + std::to_string(size);
	m_pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	m_pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
	m_pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto tcb = std::make_shared<TransformCbuf>(gfx);

	{
		Technique shade("Shade");
		{
			Step only("lambertian");

			only.AddBindable(Texture::Resolve(gfx, "Images\\brickwall.jpg"));
			only.AddBindable(Sampler::Resolve(gfx));

			auto pvs = VertexShader::Resolve(gfx, "PhongDif_VS.cso");
			auto pvsbc = pvs->GetByteCode();
			only.AddBindable(std::move(pvs));

			only.AddBindable(PixelShader::Resolve(gfx, "PhongDif_PS.cso"));

			Dynamic::RawLayout lay;
			lay.Add<Dynamic::Float3>("specularColor");
			lay.Add<Dynamic::Float>("specularWeight");
			lay.Add<Dynamic::Float>("specularGloss");
			auto buf = Dynamic::Buffer(std::move(lay));
			buf["specularColor"] = dx::XMFLOAT3{ 1.0f,1.0f,1.0f };
			buf["specularWeight"] = 0.1f;
			buf["specularGloss"] = 20.0f;
			only.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1u));

			only.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

			only.AddBindable(Rasterizer::Resolve(gfx, false));

			only.AddBindable(tcb);

			shade.AddStep(std::move(only));
		}
		AddTechnique(std::move(shade));
	}

	{
		Technique outline("Outline");
		{
			Step mask("outlineMask");

			// TODO: better sub-layout generation tech for future consideration maybe
			mask.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), VertexShader::Resolve(gfx, "Solid_VS.cso")->GetByteCode()));

			mask.AddBindable(std::move(tcb));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep(std::move(mask));
		}
		{
			Step draw("outlineDraw");

			Dynamic::RawLayout lay;
			lay.Add<Dynamic::Float4>("color");
			auto buf = Dynamic::Buffer(std::move(lay));
			buf["color"] = DirectX::XMFLOAT4{ 1.0f,0.4f,0.4f,1.0f };
			draw.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1u));

			// TODO: better sub-layout generation tech for future consideration maybe
			draw.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), VertexShader::Resolve(gfx, "Solid_VS.cso")->GetByteCode()));

			draw.AddBindable(std::make_shared<TransformCbuf>(gfx));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep(std::move(draw));
		}
		AddTechnique(std::move(outline));
	}

	DirectX::XMStoreFloat4x4(&m_Transform, DirectX::XMMatrixIdentity());
}

void TestCube::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	m_Transform._41 = pos.x;
	m_Transform._42 = pos.y;
	m_Transform._43 = pos.z;
}

void TestCube::SetRotation(float pitch, float yaw, float roll) noexcept
{
	DirectX::XMFLOAT4X4 _RotMatrix;
	DirectX::XMStoreFloat4x4(&_RotMatrix, DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll));

	m_Transform._11 = _RotMatrix._11;
	m_Transform._12 = _RotMatrix._12;
	m_Transform._13 = _RotMatrix._13;

	m_Transform._21 = _RotMatrix._21;
	m_Transform._22 = _RotMatrix._22;
	m_Transform._23 = _RotMatrix._23;

	m_Transform._31 = _RotMatrix._31;
	m_Transform._32 = _RotMatrix._32;
	m_Transform._33 = _RotMatrix._33;
}

void TestCube::SpawnControlWindow(Graphics& gfx, const std::string& name) noexcept
{
	if (ImGui::Begin(name.c_str()))
	{
		
		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
		if (ImGui::IsKeyPressed(90))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(69))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(82)) // r Key
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<const float*>(m_Transform.m), matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Translation", matrixTranslation, 3);
		ImGui::InputFloat3("Rotation", matrixRotation, 3);
		ImGui::InputFloat3("Scaling", matrixScale, 3);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(m_Transform.m));

		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}
		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		if (m_bGizmoEnabled)
		{
			DirectX::XMFLOAT4X4 _ViewMatrix;
			DirectX::XMFLOAT4X4 _ProjectionMatrix;

			DirectX::XMStoreFloat4x4(&_ViewMatrix, gfx.GetCamera());
			DirectX::XMStoreFloat4x4(&_ProjectionMatrix, gfx.GetProjection());

			ImGuizmo::Manipulate(reinterpret_cast<const float*>(_ViewMatrix.m), reinterpret_cast<const float*>(_ProjectionMatrix.m), mCurrentGizmoOperation, mCurrentGizmoMode, reinterpret_cast<float*>(m_Transform.m), nullptr, nullptr);
		}

		ImGui::Checkbox("Gizmo", &m_bGizmoEnabled);

		class Probe : public TechniqueProbe
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
				if (auto v = buf["color"]; v.Exists())
				{
					dcheck(ImGui::ColorPicker4(tag("Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT4&>(v))));
				}
				if (auto v = buf["specularIntensity"]; v.Exists())
				{
					dcheck(ImGui::SliderFloat(tag("Spec. Intens."), &v, 0.0f, 1.0f));
				}
				if (auto v = buf["specularPower"]; v.Exists())
				{
					dcheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f", 1.5f));
				}
				return dirty;
			}
		} probe;

		Accept(probe);
	}
	ImGui::End();
}

DirectX::XMMATRIX TestCube::GetTransformXM() const noexcept
{
	auto transform = DirectX::XMLoadFloat4x4(&m_Transform);
	return transform;
}
