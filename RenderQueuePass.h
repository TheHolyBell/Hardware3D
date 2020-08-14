#pragma once
#include "BindingPass.h"
#include "Job.h"
#include <vector>

namespace RenderGraph
{
	class RenderQueuePass : public BindingPass
	{
	public:
		using BindingPass::BindingPass;
		void Accept(Job job) noexcept;
		virtual void Execute(Graphics& gfx) const noxnd override;
		virtual void Reset() noxnd override;
	private:
		std::vector<Job> m_Jobs;
	};
}