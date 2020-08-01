#include "TransformCbufDoubleboi.h"

namespace Bind
{
	std::unique_ptr<PixelConstantBuffer<TransformCbuf::Transforms>> TransformCbufDoubleBoi::s_PCbuffer = nullptr;
	
	TransformCbufDoubleBoi::TransformCbufDoubleBoi(Graphics& gfx, const Drawable& parent, UINT slotV, UINT slotP)
		: TransformCbuf(gfx, parent, slotV)
	{
		if (s_PCbuffer == nullptr)
			s_PCbuffer = std::make_unique<PixelConstantBuffer<Transforms>>(gfx, slotP);
	}

	void TransformCbufDoubleBoi::Bind(Graphics& gfx) noexcept
	{
		const auto tf = GetTransforms(gfx);
		TransformCbuf::UpdateBindImpl(gfx, tf);
		UpdateBindImpl(gfx, tf);
	}

	void TransformCbufDoubleBoi::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept
	{
		s_PCbuffer->Update(gfx, tf);
		s_PCbuffer->Bind(gfx);
	}
}