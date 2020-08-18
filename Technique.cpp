#include "Technique.h"
#include "Drawable.h"
#include "TechniqueProbe.h"


void Technique::Submit(const Drawable& drawable, size_t channelFilter) const noexcept
{
	if (m_bActive && ((m_Channels & channelFilter) != 0))
	{
		for (const auto& step : m_Steps)
		{
			step.Submit(drawable);
		}
	}
}

void Technique::InitializeParentReferences(const Drawable& parent) noexcept
{
	for (auto& s : m_Steps)
	{
		s.InitializeParentReferences(parent);
	}
}

Technique::Technique(size_t channels)
	:
	m_Channels{ channels }
{}

Technique::Technique(const std::string& name, size_t channels, bool startActive) noexcept
	:
	m_bActive(startActive),
	m_Name(name),
	m_Channels(channels)
{}

void Technique::AddStep(Step step) noexcept
{
	m_Steps.push_back(std::move(step));
}

bool Technique::IsActive() const noexcept
{
	return m_bActive;
}

void Technique::SetActiveState(bool active_in) noexcept
{
	m_bActive = active_in;
}

void Technique::Accept(TechniqueProbe& probe)
{
	probe.SetTechnique(this);
	for (auto& s : m_Steps)
	{
		s.Accept(probe);
	}
}

const std::string& Technique::GetName() const noexcept
{
	return m_Name;
}

void Technique::Link(RenderGraph::RenderGraph& rg)
{
	for (auto& step : m_Steps)
	{
		step.Link(rg);
	}
}