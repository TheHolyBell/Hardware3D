#pragma once
#include "TransformCbuf.h"

namespace Bind
{
	class TransformCbufDoubleBoi : public TransformCbuf
	{
	public:
		TransformCbufDoubleBoi(Graphics& gfx, const Drawable& parent, UINT slotV = 0, UINT slotP = 0);
		virtual void Bind(Graphics& gfx) noexcept override;
	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept;
	private:
		static std::unique_ptr<PixelConstantBuffer<Transforms>> s_PCbuffer;
	};
}