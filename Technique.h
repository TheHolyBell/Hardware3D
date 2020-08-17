#pragma once
#include "Step.h"
#include <vector>

class TechniqueProbe;
class Drawable;

namespace RenderGraph
{
	class RenderGraph;
}

class Technique
{
public:
	Technique(size_t channels);
	Technique(const std::string& name, size_t channels, bool startActive = true) noexcept;

	void Submit(const Drawable& drawable, size_t channelFilter) const noexcept;
	void AddStep(Step step) noexcept;
	bool IsActive() const noexcept;
	void SetActiveState(bool active_in) noexcept;
	void InitializeParentReferences(const Drawable& parent) noexcept;
	void Accept(TechniqueProbe& probe);
	const std::string& GetName() const noexcept;
	void Link(RenderGraph::RenderGraph& renderGraph);
private:
	bool m_bActive = true;
	std::vector<Step> m_Steps;
	std::string m_Name;
	size_t m_Channels;
};