#pragma once
#include "Drawable.h"

class SolidSphere : public Drawable
{
public:
	SolidSphere(Graphics& gfx, float radius);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 m_Pos = { 1.0f, 1.0f, 1.0f };
};