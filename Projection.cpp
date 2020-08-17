#include "Projection.h"
#include "ImGui/imgui.h"
#include "Graphics.h"

Projection::Projection(Graphics& gfx, float width, float height, float nearZ, float farZ, float fov)
	:
	m_Width(width),
	m_Height(height),
	m_NearZ(nearZ),
	m_FarZ(farZ),
	m_Fov(fov),
	m_Frust(gfx, width, height, nearZ, farZ),
	m_HomeWidth(width), m_HomeHeight(height), m_HomeNearZ(nearZ), m_HomeFarZ(farZ), m_HomeFov(fov)
{}

DirectX::XMMATRIX Projection::GetMatrix() const
{
	return DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_Fov), (float)m_Width / (float)m_Height, m_NearZ, m_FarZ);
}

void Projection::RenderWidgets(Graphics& gfx)
{
	bool dirty = false;
	const auto dcheck = [&dirty](bool d) { dirty = dirty || d; };

	ImGui::Text("Projection");
	dcheck(ImGui::SliderFloat("Width", &m_Width, 0.01f, 4.0f, "%.2f", 1.5f));
	dcheck(ImGui::SliderFloat("Height", &m_Height, 0.01f, 4.0f, "%.2f", 1.5f));
	dcheck(ImGui::SliderFloat("Near Z", &m_NearZ, 0.01f, m_FarZ - 0.01f, "%.2f", 4.0f));
	dcheck(ImGui::SliderFloat("Far Z", &m_FarZ, m_NearZ + 0.01f, 400.0f, "%.2f", 4.0f));
	ImGui::SliderFloat("FOV", &m_Fov, 1.0f, 179.0f);

	if (dirty)
	{
		m_Frust.SetVertices(gfx, m_Width, m_Height, m_NearZ, m_FarZ);
	}
}

void Projection::SetPos(DirectX::XMFLOAT3 pos)
{
	m_Frust.SetPos(pos);
}

void Projection::SetRotation(DirectX::XMFLOAT3 rot)
{
	m_Frust.SetRotation(rot);
}

void Projection::Submit(size_t channel) const
{
	m_Frust.Submit(channel);
}

void Projection::LinkTechniques(RenderGraph::RenderGraph& renderGraph)
{
	m_Frust.LinkTechniques(renderGraph);
}

void Projection::Reset(Graphics& gfx)
{
	m_Width = m_HomeWidth;
	m_Height = m_HomeHeight;
	m_NearZ = m_HomeNearZ;
	m_FarZ = m_HomeFarZ;
	m_Fov = m_HomeFov;
	m_Frust.SetVertices(gfx, m_Width, m_Height, m_NearZ, m_FarZ);
}