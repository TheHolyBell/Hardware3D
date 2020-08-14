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
	Technique() = default;
	
	Technique(const std::string& name, bool startActive = true) noexcept;
	void Submit(const Drawable& drawable) const noexcept;
	void AddStep(Step step) noexcept;
	bool IsActive() const noexcept;
	void SetActiveState(bool state) noexcept;
	void InitializeParentReferences(const Drawable& parent) noexcept;
	void Accept(TechniqueProbe& probe);
	const std::string& GetName() const noexcept;
	void Link(RenderGraph::RenderGraph& renderGraph);
private:
	bool m_bActive = true;
	std::vector<Step> m_Steps;
	std::string m_Name;
};