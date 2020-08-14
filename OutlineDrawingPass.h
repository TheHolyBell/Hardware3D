#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "PixelShader.h"
#include "VertexShader.h"
#include "Stencil.h"
#include "Rasterizer.h"

class Graphics;

namespace RenderGraph
{
	class OutlineDrawingPass : public RenderQueuePass
	{
	public:
		OutlineDrawingPass(Graphics& gfx, const std::string& name)
			:
			RenderQueuePass(name)
		{
			using namespace Bind;
			RegisterSink(DirectBufferSink<Bind::RenderTarget>::Make("renderTarget", m_RenderTarget));
			RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", m_DepthStencil));
			RegisterSource(DirectBufferSource<Bind::RenderTarget>::Make("renderTarget", m_RenderTarget));
			RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", m_DepthStencil));
			AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
			AddBind(PixelShader::Resolve(gfx, "Solid_PS.cso"));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));
			AddBind(Rasterizer::Resolve(gfx, false));
		}
	};
}