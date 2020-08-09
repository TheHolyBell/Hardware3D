#pragma once

#include "Graphics.h"
#include "ConditionalNoexcept.h"
#include "GraphicsResource.h"
#include <string>
#include <memory>

class Drawable;
class TechniqueProbe;

namespace Bind
{
	class Bindable : public GraphicsResource
	{
	public:
		virtual void Bind(Graphics& gfx) noexcept = 0;
		virtual void InitializeParentReference(const Drawable& parent) noexcept
		{}
		virtual void Accept(TechniqueProbe& probe)
		{}
		virtual std::string GetUID() const noexcept
		{
			assert(false && "This class didn't override GetUID() method");
			return "You fucked up";
		}
		virtual ~Bindable() = default;
	};

	class CloningBindable : public Bindable
	{
	public:
		virtual std::unique_ptr<CloningBindable> Clone() const noexcept = 0;
	};
}