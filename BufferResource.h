#pragma once
#include "ConditionalNoexcept.h"

class Graphics;

namespace Bind
{
	class BufferResource
	{
	public:
		virtual ~BufferResource() = default;

		virtual void BindAsBuffer(Graphics& gfx) noxnd = 0;
		virtual void BindAsBuffer(Graphics&, BufferResource*) noxnd = 0;
		virtual void Clear(Graphics& gfx) noxnd = 0;
	};
}