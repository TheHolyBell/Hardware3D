#include "PointLight.h"
#include "ImGui\imgui.h"

PointLight::PointLight(Graphics& gfx, DirectX::XMFLOAT3 pos, float radius)
	: m_Mesh(gfx, radius), m_CBuffer(gfx)
{
	m_Home = {
		pos,
		{ 0.05f,0.05f,0.05f },
		{ 1.0f,1.0f,1.0f },
		1.0f,
		1.0f,
		0.045f,
		0.0075f,
	};
	Reset();
	m_pCamera = std::make_shared<Camera>(gfx, "Light", m_cbData.pos, 0.0f, 0.0f, true);
}

void PointLight::SpawnControlWindow(Graphics& gfx) noexcept
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

		ImGui::Checkbox("Gizmo", &m_bGuizmoEnabled);

		if (m_bGuizmoEnabled)
			m_Mesh.RenderGizmo(gfx);

		m_cbData.pos = m_Mesh.GetPos();
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}



void PointLight::Reset() noexcept
{
	m_cbData = m_Home;
}

void PointLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept
{
	auto dataCopy = m_cbData;
	const auto pos = DirectX::XMLoadFloat3(&m_cbData.pos);
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));
	m_CBuffer.Update(gfx, dataCopy);
	m_CBuffer.Bind(gfx);
}

void PointLight::Submit(size_t channels) const noxnd
{
	m_Mesh.SetPos(m_cbData.pos);
	m_Mesh.Submit(channels);
}

void PointLight::LinkTechniques(RenderGraph::RenderGraph& renderGraph)
{
	m_Mesh.LinkTechniques(renderGraph);
}

std::shared_ptr<Camera> PointLight::ShareCamera() const noexcept
{
	return m_pCamera;
}
