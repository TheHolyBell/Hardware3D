#pragma once
#include "Drawable.h"

class TestPlane : public Drawable
{
public:
	TestPlane(Graphics& gfx, float size, DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,0.0f });
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float pitch, float yaw, float roll) noexcept;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;

	void SpawnControlWindow(Graphics& gfx, const std::string& name) noexcept;

private:
	struct PSMaterialConstant
	{
		DirectX::XMFLOAT4 color;
	}  m_Pmc;
	DirectX::XMFLOAT4X4 m_Transform;
	bool m_bGizmoEnabled = false;
};