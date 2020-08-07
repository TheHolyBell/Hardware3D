#include "Step.h"
#include "Drawable.h"
#include "FrameCommander.h"

void Step::Submit(FrameCommander& frame, const Drawable& drawable) const
{
	frame.Accept(Job{ this, &drawable }, m_TargetPass);
}

void Step::InitializeParentReferences(const Drawable& parent) noexcept
{
	for (auto& b : m_Bindables)
		b->InitializeParentReference(parent);
}