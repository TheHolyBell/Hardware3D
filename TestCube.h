#pragma once
#include "Drawable.h"
#include "Bindable.h"
#include "IndexBuffer.h"

class TestCube : public Drawable
{
public:
	TestCube(Graphics& gfx, float size);

	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float pitch, float yaw, float roll) noexcept;
	void SpawnControlWindow(Graphics& gfx, const std::string& name) noexcept;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT4X4 m_Transform;
	bool m_bGizmoEnabled = false;
};