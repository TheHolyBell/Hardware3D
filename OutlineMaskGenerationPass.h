#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "NullPixelShader.h"
#include "VertexShader.h"
#include "Stencil.h"
#include "Rasterizer.h"

class Graphics;

namespace RenderGraph
{
	class OutlineMaskGenerationPass : public RenderQueuePass
	{
	public:
		OutlineMaskGenerationPass(Graphics& gfx, const std::string& name)
			: RenderQueuePass(name)
		{
			using namespace Bind;
			RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", m_DepthStencil));
			RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", m_DepthStencil));
			AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
			AddBind(NullPixelShader::Resolve(gfx));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Write));
			AddBind(Rasterizer::Resolve(gfx, false));
		}
	};
}