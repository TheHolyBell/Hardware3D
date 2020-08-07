#pragma once
#include "Step.h"
#include "TechniqueProbe.h"
#include <vector>

class Technique
{
public:
	Technique() = default;
	Technique(std::string name, bool startActive = true) noexcept
		: m_Name(name), m_bActive(startActive)
	{}
	void Submit(class FrameCommander& frame, const class Drawable& drawable) const noexcept;
	void AddStep(Step step) noexcept
	{
		m_Steps.push_back(std::move(step));
	}
	bool IsActive() const noexcept
	{
		return m_bActive;
	}
	void SetActiveState(bool state) noexcept
	{
		m_bActive = state;
	}
	void InitializeParentReferences(const class Drawable& parent) noexcept;
	void Accept(TechniqueProbe& probe)
	{
		probe.SetTechnique(this);
		for (auto& s : m_Steps)
			s.Accept(probe);
	}
	const std::string& GetName() const noexcept
	{
		return m_Name;
	}
private:
	bool m_bActive = true;
	std::vector<Step> m_Steps;
	std::string m_Name = "Nameless Tech";
};