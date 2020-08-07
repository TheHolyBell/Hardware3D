#include "TransformCbuf.h"
#include "Drawable.h"
#include "Graphics.h"

namespace Bind
{
	TransformCbuf::TransformCbuf(Graphics& gfx, UINT slot)
	{
		if (!s_pVCBuffer)
		{
			s_pVCBuffer = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}

	void TransformCbuf::InitializeParentReference(const Drawable& parent) noexcept
	{
		m_pParent = &parent;
	}

	void TransformCbuf::Bind(Graphics& gfx) noexcept
	{
		UpdateBindImpl(gfx, GetTransforms(gfx));
	}

	void TransformCbuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept
	{
		s_pVCBuffer->Update(gfx, tf);
		s_pVCBuffer->Bind(gfx);
	}

	TransformCbuf::Transforms TransformCbuf::GetTransforms(Graphics& gfx) noexcept
	{
		const auto modelView = m_pParent->GetTransformXM() * gfx.GetCamera();
		return {
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(
				modelView *
				gfx.GetProjection())
			};
	}

	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::s_pVCBuffer;
}