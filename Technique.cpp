#include "Technique.h"
#include "Drawable.h"
#include "FrameCommander.h"

void Technique::Submit(FrameCommander& frame, const Drawable& drawable) const noexcept
{
	if (m_bActive)
	{
		for (const auto& step : m_Steps)
		{
			step.Submit(frame, drawable);
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