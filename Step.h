#pragma once
#include <vector>
#include <memory>
#include "Bindable.h"
#include "Graphics.h"
#include "TechniqueProbe.h"

class Step
{
public:
	Step(size_t targetPass)
		: m_TargetPass(targetPass)
	{}

	Step(Step&& rhs) = default;

	Step(const Step& rhs) noexcept
		: m_TargetPass(rhs.m_TargetPass)
	{
		m_Bindables.reserve(rhs.m_Bindables.size());

		for (auto& pb : rhs.m_Bindables)
		{
			if (auto* pCloning = dynamic_cast<const Bind::CloningBindable*>(pb.get()))
				m_Bindables.push_back(pCloning->Clone());
			else
				m_Bindables.push_back(pb);
		}
	}

	Step& operator=(const Step& rhs) = delete;
	Step& operator=(Step&& rhs) = delete;

	void AddBindable(std::shared_ptr<Bind::Bindable> bind) noexcept
	{
		m_Bindables.push_back(std::move(bind));
	}
	void Submit(class FrameCommander& frame, const class Drawable& drawable) const;
	void Bind(Graphics& gfx) const
	{
		for (const auto& b : m_Bindables)
			b->Bind(gfx);
	}
	void InitializeParentReferences(const class Drawable& parent) noexcept;
	void Accept(TechniqueProbe& probe)
	{
		probe.SetStep(this);
		for (auto& pb : m_Bindables)
			pb->Accept(probe);
	}
private:
	size_t m_TargetPass;
	std::vector<std::shared_ptr<Bind::Bindable>> m_Bindables;
};