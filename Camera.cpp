#include "Camera.h"
#include "ImGui\imgui.h"
#include "ChiliMath.h"
#include <algorithm>
#include "Graphics.h"

namespace dx = DirectX;

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	using namespace DirectX;

	auto translation = XMMatrixTranslation(-m_Pos.x, -m_Pos.y, -m_Pos.z);
	auto rotation = XMMatrixTranspose(XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, 0.0f));

	return translation * rotation;
}

Camera::Camera(Graphics& gfx, const std::string& name, DirectX::XMFLOAT3 homePos, float homePitch, float homeYaw, bool tethered) noexcept
	:
	m_Name(std::move(name)),
	m_HomePos(homePos),
	m_HomePitch(homePitch),
	m_HomeYaw(homeYaw),
	m_Projection(gfx, 1.0f, 9.0f / 16.0f, 0.5f, 400.0f),
	m_Indicator(gfx),
	m_bTethered(tethered)
{
	if (tethered)
	{
		m_Pos = homePos;
		m_Indicator.SetPos(m_Pos);
		m_Projection.SetPos(m_Pos);
	}
	Reset(gfx);
}

void Camera::BindToGraphics(Graphics& gfx) const
{
	gfx.SetCamera(GetMatrix());
	gfx.SetProjection(m_Projection.GetMatrix());
}

DirectX::XMMATRIX Camera::GetProjection() const noexcept
{
	return m_Projection.GetMatrix();
}

void Camera::SpawnControlWidgets(Graphics& gfx) noexcept
{
	bool rotDirty = false;
	bool posDirty = false;
	const auto dcheck = [](bool d, bool& carry) { carry = carry || d; };
	if (!m_bTethered)
	{
		ImGui::Text("Position");
		dcheck(ImGui::SliderFloat("X", &m_Pos.x, -80.0f, 80.0f, "%.1f"), posDirty);
		dcheck(ImGui::SliderFloat("Y", &m_Pos.y, -80.0f, 80.0f, "%.1f"), posDirty);
		dcheck(ImGui::SliderFloat("Z", &m_Pos.z, -80.0f, 80.0f, "%.1f"), posDirty);
	}
	ImGui::Text("Orientation");
	dcheck(ImGui::SliderAngle("Pitch", &m_Pitch, 0.995f * -90.0f, 0.995f * 90.0f), rotDirty);
	dcheck(ImGui::SliderAngle("Yaw", &m_Yaw, -180.0f, 180.0f), rotDirty);
	m_Projection.RenderWidgets(gfx);
	ImGui::Checkbox("Camera Indicator", &m_bEnableCameraIndicator);
	ImGui::Checkbox("Frustum Indicator", &m_bEnableFrustumIndicator);
	if (ImGui::Button("Reset"))
	{
		Reset(gfx);
	}

	if (rotDirty)
	{
		const dx::XMFLOAT3 angles = { m_Pitch,m_Yaw,0.0f };
		m_Indicator.SetRotation(angles);
		m_Projection.SetRotation(angles);
	}
	if (posDirty)
	{
		m_Indicator.SetPos(m_Pos);
		m_Projection.SetPos(m_Pos);
	}
}

void Camera::Reset(Graphics& gfx) noexcept
{
	if (!m_bTethered)
	{
		m_Pos = m_HomePos;
		m_Indicator.SetPos(m_Pos);
		m_Projection.SetPos(m_Pos);
	}
	m_Pitch = m_HomePitch;
	m_Yaw = m_HomeYaw;

	const dx::XMFLOAT3 angles = { m_Pitch,m_Yaw,0.0f };
	m_Indicator.SetRotation(angles);
	m_Projection.SetRotation(angles);
	m_Projection.Reset(gfx);
}

void Camera::Rotate(float dx, float dy) noexcept
{
	m_Yaw = wrap_angle(m_Yaw + dx * s_RotationSpeed);
	m_Pitch = std::clamp(m_Pitch + dy * s_RotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
	const dx::XMFLOAT3 angles = { m_Pitch,m_Yaw,0.0f };
	m_Indicator.SetRotation(angles);
	m_Projection.SetRotation(angles);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	if (!m_bTethered)
	{
		dx::XMStoreFloat3(&translation, dx::XMVector3Transform(
			dx::XMLoadFloat3(&translation),
			dx::XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, 0.0f) *
			dx::XMMatrixScaling(s_TravelSpeed, s_TravelSpeed, s_TravelSpeed)
		));
		m_Pos = {
			m_Pos.x + translation.x,
			m_Pos.y + translation.y,
			m_Pos.z + translation.z
		};
		m_Indicator.SetPos(m_Pos);
		m_Projection.SetPos(m_Pos);
	}
}

DirectX::XMFLOAT3 Camera::GetPos() const noexcept
{
	return m_Pos;
}

void Camera::SetPos(const DirectX::XMFLOAT3& pos) noexcept
{
	m_Pos = pos;
	m_Indicator.SetPos(pos);
	m_Projection.SetPos(pos);
}

const std::string& Camera::GetName() const noexcept
{
	return m_Name;
}

void Camera::LinkTechniques(RenderGraph::RenderGraph& rg)
{
	m_Indicator.LinkTechniques(rg);
	m_Projection.LinkTechniques(rg);
}

void Camera::Submit(size_t channels) const
{
	if (m_bEnableCameraIndicator)
	{
		m_Indicator.Submit(channels);
	}
	if (m_bEnableFrustumIndicator)
	{
		m_Projection.Submit(channels);
	}
}