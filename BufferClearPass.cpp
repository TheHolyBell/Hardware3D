#include "BufferClearPass.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "Sink.h"
#include "Source.h"


namespace RenderGraph
{
	BufferClearPass::BufferClearPass(const std::string& name)
		:
		Pass(name)
	{
		RegisterSink(DirectBufferSink<Bind::BufferResource>::Make("buffer", m_Buffer));
		RegisterSource(DirectBufferSource<Bind::BufferResource>::Make("buffer", m_Buffer));
	}

	void BufferClearPass::Execute(Graphics& gfx) const noxnd
	{
		m_Buffer->Clear(gfx);
	}
}