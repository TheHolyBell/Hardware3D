#pragma once
#include "DrawableBase.h"

class SolidSphere : public DrawableBase<SolidSphere>
{
public:
	SolidSphere(Graphics& gfx, float radius);

	virtual void Update(float dt) noexcept override;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;

	void SetPos(DirectX::XMFLOAT3 pos) noexcept;

private:
	DirectX::XMFLOAT3 m_Position = { 1.0f, 1.0f, 1.0f };
};