#include "Source.h"
#include "RenderGraphCompileException.h"
#include <algorithm>
#include <cctype>

namespace RenderGraph
{
	Source::Source(const std::string& nameIn)
		:
		m_Name(nameIn)
	{
		if (m_Name.empty())
		{
			throw RGC_EXCEPTION("Empty output name");
		}
		const bool nameCharsValid = std::all_of(m_Name.begin(), m_Name.end(), [](char c) {
			return std::isalnum(c) || c == '_';
		});
		if (!nameCharsValid || std::isdigit(m_Name.front()))
		{
			throw RGC_EXCEPTION("Invalid output name: " + m_Name);
		}
	}

	std::shared_ptr<Bind::Bindable> Source::YieldBindable()
	{
		throw RGC_EXCEPTION("Output cannot be accessed as bindable");
	}

	std::shared_ptr<Bind::BufferResource> Source::YieldBuffer()
	{
		throw RGC_EXCEPTION("Output cannot be accessed as buffer");
	}

	const std::string& Source::GetName() const noexcept
	{
		return m_Name;
	}
}