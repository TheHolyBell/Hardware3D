#pragma once
#include "Graphics.h"

class Camera
{
public:
	Camera(DirectX::XMFLOAT3 homePos = {}, float homePitch = 0.0f, float homeYaw = 0.0f) noexcept; 
	DirectX::XMMATRIX GetMatrix() const noexcept;
	DirectX::XMFLOAT3 GetPos() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
private:
	DirectX::XMFLOAT3 m_HomePos;
	float m_HomePitch;
	float m_HomeYaw;

	DirectX::XMFLOAT3 m_Pos;
	float m_Pitch;
	float m_Yaw;

	static constexpr float s_TravelSpeed = 20.0f;
	static constexpr float s_RotationSpeed = 0.004f;
};