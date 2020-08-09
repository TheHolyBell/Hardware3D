#pragma once
#include <vector>
#include <memory>
#include "Bindable.h"
#include "Graphics.h"
#include "TechniqueProbe.h"

class Step
{
public:
	Step(size_t targetPass_in)
		:
		m_TargetPass{ targetPass_in }
	{}
	Step(Step&&) = default;
	Step(const Step& src) noexcept
		:
		m_TargetPass(src.m_TargetPass)
	{
		m_Bindables.reserve(src.m_Bindables.size());
		for (auto& pb : src.m_Bindables)
		{
			if (auto* pCloning = dynamic_cast<const Bind::CloningBindable*>(pb.get()))
			{
				m_Bindables.push_back(pCloning->Clone());
			}
			else
			{
				m_Bindables.push_back(pb);
			}
		}
	}
	Step& operator=(const Step&) = delete;
	Step& operator=(Step&&) = delete;
	void AddBindable(std::shared_ptr<Bind::Bindable> bind_in) noexcept
	{
		m_Bindables.push_back(std::move(bind_in));
	}
	void Submit(class FrameCommander& frame, const class Drawable& drawable) const;
	void Bind(Graphics& gfx) const
	{
		for (const auto& b : m_Bindables)
		{
			b->Bind(gfx);
		}
	}
	void InitializeParentReferences(const class Drawable& parent) noexcept;
	void Accept(TechniqueProbe& probe)
	{
		probe.SetStep(this);
		for (auto& pb : m_Bindables)
		{
			pb->Accept(probe);
		}
	}
private:
	size_t m_TargetPass;
	std::vector<std::shared_ptr<Bind::Bindable>> m_Bindables;
};