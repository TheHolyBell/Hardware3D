#pragma once
#include <vector>
#include <memory>
#include "Bindable.h"
#include "Graphics.h"

class TechniqueProbe;
class Drawable;

namespace RenderGraph
{
	class RenderQueuePass;
	class RenderGraph;
}

class Step
{
public:
	Step(const std::string& targetPassName);
	 
	Step(Step&& rhs) = default;
	Step(const Step& rhs) noexcept;

	Step& operator=(const Step& rhs) = delete;
	Step& operator=(Step&& rhs) = delete;

	void AddBindable(std::shared_ptr<Bind::Bindable> bind) noexcept;
	void Submit(const Drawable& drawable) const;
	void Bind(Graphics& gfx) const noxnd;
	void InitializeParentReferences(const Drawable& parent) noexcept;
	void Accept(TechniqueProbe& probe);
	void Link(RenderGraph::RenderGraph& renderGraph);
private:
	std::vector<std::shared_ptr<Bind::Bindable>> m_Bindables;
	RenderGraph::RenderQueuePass* m_pTargetPass = nullptr;
	std::string m_TargetPassName;
};