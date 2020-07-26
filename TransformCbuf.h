#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>
#include <memory>

class TransformCbuf : public Bindable
{
public:
	TransformCbuf(Graphics& gfx, const Drawable& parent);
	virtual void Bind(Graphics& gfx) noexcept override;
private:
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> s_pVCBuffer;
	const Drawable& m_Parent;
};