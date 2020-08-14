#include "RenderQueuePass.h"

namespace RenderGraph
{
	void RenderQueuePass::Accept(Job job) noexcept
	{
		m_Jobs.push_back(job);
	}

	void RenderQueuePass::Execute(Graphics& gfx) const noxnd
	{
		BindAll(gfx);

		for (const auto& j : m_Jobs)
			j.Execute(gfx);
	}

	void RenderQueuePass::Reset() noxnd
	{
		m_Jobs.clear();
	}
}