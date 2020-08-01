#include "TestCube.h"
#include "Cube.h"
#include "BindableCommon.h"
#include "TransformCbufDoubleboi.h"
#include "ImGui\imgui.h"

TestCube::TestCube(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Cube::MakeIndependentTextured();
	model.Transform(dx::XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();

	const auto geometryTag = "$cube." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	AddBind(Texture::Resolve(gfx, "Images/brickwall.jpg"));
	AddBind(Texture::Resolve(gfx, "Images/brickwall_normal.jpg", 1));

	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "PhongPSNormalMap.cso"));

	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, m_Pmc, 1));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbufDoubleBoi>(gfx, *this, 0, 2));
}

void TestCube::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	m_Pos = pos;
}

void TestCube::SetRotation(float pitch, float yaw, float roll) noexcept
{
	m_Pitch = pitch;
	m_Yaw = yaw;
	m_Roll = roll;
}

DirectX::XMMATRIX TestCube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, m_Roll) *
		DirectX::XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
}

void TestCube::SpawnControlWindow(Graphics& gfx) noexcept
{
	if (ImGui::Begin("Cube"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &m_Pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &m_Pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &m_Pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &m_Pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &m_Yaw, -180.0f, 180.0f);
		ImGui::SliderAngle("Roll", &m_Roll, -180.0f, 180.0f);
		ImGui::Text("Shading");
		bool changed0 = ImGui::SliderFloat("Spec. Int.", &m_Pmc.specularIntensity, 0.0f, 1.0f);
		bool changed1 = ImGui::SliderFloat("Spec. Power", &m_Pmc.specularPower, 0.0f, 100.0f);
		bool checkState = m_Pmc.normalMappingEnabled == TRUE;
		bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		m_Pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
		if (changed0 || changed1 || changed2)
		{
			QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, m_Pmc);
		}
	}
	ImGui::End();
}
