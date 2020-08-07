#pragma once
#include "Graphics.h"
#include "Job.h"
#include <vector>

class Pass
{
public:
	void Accept(Job job) noexcept
	{
		m_Jobs.push_back(job);
	}
	void Execute(Graphics& gfx) const noxnd
	{
		for (const auto& j : m_Jobs)
			j.Execute(gfx);
	}
	void Reset() noexcept
	{
		m_Jobs.clear();
	}
private:
	std::vector<Job> m_Jobs;
};