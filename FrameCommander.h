#pragma once
#include <array>
#include "BindableCommon.h"
#include "Graphics.h"
#include "Job.h"
#include "Pass.h"
#include "PerfLog.h"

class FrameCommander
{
public:
	void Accept(Job job, size_t target) noexcept
	{
		m_Passes[target].Accept(job);
	}
	void Execute(Graphics& gfx) const noxnd
	{
		using namespace Bind;

		// Normally this would be a loop with each pass defining it setup / etc.
		// and later on it would be a complex graph with parallel execution contingent
		// or input / output requirements

		// Main phong lighting pass
		Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
		m_Passes[0].Execute(gfx);
		// Outline masking pass
		Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
		NullPixelShader::Resolve(gfx)->Bind(gfx);
		m_Passes[1].Execute(gfx);
		// Outline drawing pass
		Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
		m_Passes[2].Execute(gfx);
	}
	void Reset() noexcept
	{
		for (auto& p : m_Passes)
			p.Reset();
	}
private:
	std::array<Pass, 3> m_Passes;
};