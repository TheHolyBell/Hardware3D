#pragma once
#include "Drawable.h"

class TestCube : public Drawable
{
public:
	TestCube(Graphics& gfx, float size);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float pitch, float yaw, float roll) noexcept;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;

	void SpawnControlWindow(Graphics& gfx) noexcept;
private:
	struct PSMaterialConstant
	{
		float specularIntensity = 0.1f;
		float specularPower = 20.0f;
		BOOL normalMappingEnabled = TRUE;
		float padding[1];
	} m_Pmc;
	DirectX::XMFLOAT3 m_Pos = { 1.0f, 1.0f, 1.0f };
	float m_Pitch = 0.0f;
	float m_Yaw = 0.0f;
	float m_Roll = 0.0f;
};