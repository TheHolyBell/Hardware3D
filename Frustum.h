#pragma once
#include "Drawable.h"

class Frustum : public Drawable
{
public:
	Frustum(Graphics& gfx, float width, float height, float nearZ, float farZ);
	void SetVertices(Graphics& gfx, float width, float height, float nearZ, float farZ);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(DirectX::XMFLOAT3 rot) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 m_Pos = {};
	DirectX::XMFLOAT3 m_Rot = {};
};