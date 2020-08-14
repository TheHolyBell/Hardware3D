#pragma once
#include "FullscreenPass.h"
#include "ConstantBuffersEx.h"


class Graphics;
namespace Bind
{
	class PixelShader;
	class RenderTarget;
}

namespace RenderGraph
{
	class HorizontalBlurPass : public FullscreenPass
	{
	public:
		HorizontalBlurPass(const std::string& name, Graphics& gfx, unsigned int fullWidth, unsigned int fullHeight);
		virtual void Execute(Graphics& gfx) const noxnd override;
	private:
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> m_Direction;
	};
}