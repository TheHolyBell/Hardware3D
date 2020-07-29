#pragma once
#include "Graphics.h"

class Camera
{
public:
	Camera() noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
private:
	DirectX::XMFLOAT3 m_Pos;
	float m_Pitch;
	float m_Yaw;

	static constexpr float s_TravelSpeed = 12.0f;
	static constexpr float s_RotationSpeed = 0.004f;
};