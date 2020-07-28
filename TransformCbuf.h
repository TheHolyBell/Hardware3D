#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>
#include <memory>

class TransformCbuf : public Bindable
{
private:
	struct Transforms
	{
		DirectX::XMMATRIX model;
		DirectX::XMMATRIX MVP;
	};
public:
	TransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot = 0);
	virtual void Bind(Graphics& gfx) noexcept override;
private:
	static std::unique_ptr<VertexConstantBuffer<Transforms>> s_pVCBuffer;
	const Drawable& m_Parent;
};