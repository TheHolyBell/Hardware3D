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
	class VerticalBlurPass : public FullscreenPass
	{
	public:
		VerticalBlurPass(const std::string& name, Graphics& gfx);
		virtual void Execute(Graphics& gfx) const noxnd override;
	private:
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> m_Direction;
	};
}