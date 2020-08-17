#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"

class Graphics;

namespace RenderGraph
{
	class WireframePass : public RenderQueuePass
	{
	public:
		WireframePass(Graphics& gfx, const std::string& name)
			: RenderQueuePass(name)
		{
			using namespace Bind;
			RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", m_RenderTarget));
			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", m_DepthStencil));
			RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", m_RenderTarget));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", m_DepthStencil));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::DepthReversed));
		}
	};
}