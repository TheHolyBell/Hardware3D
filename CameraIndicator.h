#pragma once
#include "Drawable.h"

class CameraIndicator : public Drawable
{
public:
	CameraIndicator(Graphics& gfx);

	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(DirectX::XMFLOAT3 rot) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 m_Pos = {};
	DirectX::XMFLOAT3 m_Rot = {};

};