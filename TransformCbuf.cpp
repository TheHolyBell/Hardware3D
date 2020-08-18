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

	void TransformCbuf::Bind(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(UpdateBindImpl(gfx, GetTransforms(gfx)));
	}

	void TransformCbuf::InitializeParentReference(const Drawable& parent) noexcept
	{
		m_pParent = &parent;
	}

	std::unique_ptr<CloningBindable> TransformCbuf::Clone() const noexcept
	{
		return std::make_unique<TransformCbuf>(*this);
	}

	void TransformCbuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noxnd
	{
		assert(m_pParent != nullptr);
		s_pVCBuffer->Update(gfx, tf);
		s_pVCBuffer->Bind(gfx);
	}

	TransformCbuf::Transforms TransformCbuf::GetTransforms(Graphics& gfx) noxnd
	{
		assert(m_pParent != nullptr);
		const auto model = m_pParent->GetTransformXM();
		const auto modelView = model * gfx.GetCamera();
		return {
			DirectX::XMMatrixTranspose(model),
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(
				modelView *
				gfx.GetProjection()
			)
		};
	}

	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::s_pVCBuffer;
}