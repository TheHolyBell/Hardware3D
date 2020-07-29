#include "PointLight.h"
#include "ImGui\imgui.h"

PointLight::PointLight(Graphics& gfx, float radius)
	: m_Mesh(gfx, radius), m_CBuffer(gfx)
{
	Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &m_cbData.pos.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &m_cbData.pos.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &m_cbData.pos.z, -60.0f, 60.0f, "%.1f");

		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &m_cbData.diffuseIntensity, 0.01f, 2.0f, "%.2f", 2);
		ImGui::ColorEdit3("Diffuse Color", &m_cbData.diffuseColor.x);
		ImGui::ColorEdit3("Ambient", &m_cbData.ambient.x);

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &m_cbData.attConst, 0.05f, 10.0f, "%.2f", 4);
		ImGui::SliderFloat("Linear", &m_cbData.attLin, 0.0001f, 4.0f, "%.4f", 8);
		ImGui::SliderFloat("Quadratic", &m_cbData.attQuad, 0.0000001f, 10.0f, "%.7f", 10);

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	m_cbData = {
		{ 1.5f,14.0f,-4.5f },
		{ 0.05f,0.05f,0.05f },
		{ 1.0f,1.0f,1.0f },
		1.0f,
		1.0f,
		0.045f,
		0.0075f,
	};
}

void PointLight::Draw(Graphics& gfx) const noexcept(!IS_DEBUG)
{
	m_Mesh.SetPos(m_cbData.pos);
	m_Mesh.Draw(gfx);
}

void PointLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept
{
	auto dataCopy = m_cbData;
	const auto pos = DirectX::XMLoadFloat3(&m_cbData.pos);
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));
	m_CBuffer.Update(gfx, dataCopy);
	m_CBuffer.Bind(gfx);
}
