#include "TransformCbuf.h"

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCbuf::s_pVCBuffer;

TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent)
	: m_Parent(parent)
{
	if (s_pVCBuffer == nullptr)
		s_pVCBuffer = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx);
}

void TransformCbuf::Bind(Graphics& gfx) noexcept
{
	s_pVCBuffer->Update(gfx,
		DirectX::XMMatrixTranspose(
			m_Parent.GetTransformXM() * gfx.GetProjection()
		));

	s_pVCBuffer->Bind(gfx);
}
