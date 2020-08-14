#include "Camera.h"
#include "ImGui\imgui.h"
#include "ChiliMath.h"
#include <algorithm>

namespace dx = DirectX;

Camera::Camera(DirectX::XMFLOAT3 homePos, float homePitch, float homeYaw) noexcept
	: m_HomePos(homePos), m_HomePitch(homePitch), m_HomeYaw(homeYaw)
{
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	using namespace DirectX;

	auto translation = XMMatrixTranslation(-m_Pos.x, -m_Pos.y, -m_Pos.z);
	auto rotation = XMMatrixTranspose(XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, 0.0f));

	return translation * rotation;
}

DirectX::XMFLOAT3 Camera::GetPos() const noexcept
{
	return m_Pos;
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &m_Pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &m_Pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &m_Pos.z, -80.0f, 80.0f, "%.1f");

		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &m_Pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &m_Yaw, -180.0f, 180.0f);

		if (ImGui::Button("Reset"))
			Reset();
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	m_Pos = m_HomePos;
	m_Pitch = m_HomePitch;
	m_Yaw = m_HomeYaw;
}

void Camera::Rotate(float dx, float dy) noexcept
{
	m_Yaw = wrap_angle(m_Yaw + dx * s_RotationSpeed);
	m_Pitch = std::clamp(m_Pitch + dy * s_RotationSpeed, -PI / 2.0f, PI / 2.0f);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	dx::XMStoreFloat3(&translation, dx::XMVector3Transform(
		dx::XMLoadFloat3(&translation),
		dx::XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, 0.0f) *
		dx::XMMatrixScaling(s_TravelSpeed, s_TravelSpeed, s_TravelSpeed)
	));

	m_Pos =
	{
		m_Pos.x + translation.x,
		m_Pos.y + translation.y,
		m_Pos.z + translation.z
	};
}
