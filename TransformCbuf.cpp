#include "TransformCbuf.h"

namespace Bind
{
	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::s_pVCBuffer;

	TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot)
		: m_Parent(parent)
	{
		if (s_pVCBuffer == nullptr)
			s_pVCBuffer = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
	}

	void TransformCbuf::Bind(Graphics& gfx) noexcept
	{
		const auto modelView = m_Parent.GetTransformXM() * gfx.GetCamera();
		Transforms tf;
		tf.model = DirectX::XMMatrixTranspose(modelView);
		tf.MVP = DirectX::XMMatrixTranspose(modelView * gfx.GetProjection());

		s_pVCBuffer->Update(gfx, tf);
		s_pVCBuffer->Bind(gfx);
	}
}