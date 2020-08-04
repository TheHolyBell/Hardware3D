#pragma once
#include "Drawable.h"

class SolidSphere : public Drawable
{
public:
	SolidSphere(Graphics& gfx, float radius);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	DirectX::XMFLOAT3 GetPos() const noexcept;
	void RenderGizmo(Graphics& gfx) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT4X4 m_Transformation;
};