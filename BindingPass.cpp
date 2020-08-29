#include "BindingPass.h"
#include "Bindable.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "RenderGraphCompileException.h"

namespace RenderGraph
{
	BindingPass::BindingPass(const std::string& name, std::vector<std::shared_ptr<Bind::Bindable>> binds)
		:
		Pass(name),
		m_Binds(std::move(binds))
	{}

	void BindingPass::AddBind(std::shared_ptr<Bind::Bindable> bind) noexcept
	{
		m_Binds.push_back(std::move(bind));
	}

	void BindingPass::BindAll(Graphics& gfx) const noxnd
	{
		BindBufferResources(gfx);
		for (auto& bind : m_Binds)
		{
			bind->Bind(gfx);
		}
	}

	void BindingPass::Finalize()
	{
		Pass::Finalize();
		if (!m_RenderTarget && !m_DepthStencil)
		{
			throw RGC_EXCEPTION("BindingPass [" + GetName() + "] needs at least one of a renderTarget or depthStencil");
		}
	}

	void BindingPass::BindBufferResources(Graphics& gfx) const noxnd
	{
		if (m_RenderTarget)
		{
			m_RenderTarget->BindAsBuffer(gfx, m_DepthStencil.get());
		}
		else
		{
			m_DepthStencil->BindAsBuffer(gfx);
		}
	}
}
