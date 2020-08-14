#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "PixelShader.h"
#include "VertexShader.h"
#include "Stencil.h"
#include "Rasterizer.h"
#include "Source.h"
#include "RenderTarget.h"
#include "Blender.h"

class Graphics;

namespace RenderGraph
{
	class BlurOutlineDrawingPass : public RenderQueuePass
	{
	public:
		BlurOutlineDrawingPass(Graphics& gfx, const std::string& name, unsigned int fullWidth, unsigned int fullHeight)
			:
			RenderQueuePass(name)
		{
			using namespace Bind;
			m_RenderTarget = std::make_unique<ShaderInputRenderTarget>(gfx, fullWidth / 2, fullHeight / 2, 0);
			AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
			AddBind(PixelShader::Resolve(gfx, "Solid_PS.cso"));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));
			AddBind(Blender::Resolve(gfx, false));
			RegisterSource(DirectBindableSource<Bind::RenderTarget>::Make("scratchOut", m_RenderTarget));
		}
		void Execute(Graphics& gfx) const noxnd override
		{
			m_RenderTarget->Clear(gfx);
			RenderQueuePass::Execute(gfx);
		}
	};
}