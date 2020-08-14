#include "Sink.h"
#include "Source.h"
#include "RenderGraphCompileException.h"
#include "ChiliUtil.h"
#include "BufferResource.h"
#include <sstream>
#include <cctype>
#include <algorithm>

namespace RenderGraph
{
	Sink::Sink(const std::string& registeredNameIn)
		:
		m_RegisteredName(std::move(registeredNameIn))
	{
		if (m_RegisteredName.empty())
		{
			throw RGC_EXCEPTION("Empty output name");
		}
		const bool nameCharsValid = std::all_of(m_RegisteredName.begin(), m_RegisteredName.end(), [](char c) {
			return std::isalnum(c) || c == '_';
		});
		if (!nameCharsValid || std::isdigit(m_RegisteredName.front()))
		{
			throw RGC_EXCEPTION("Invalid output name: " + m_RegisteredName);
		}
	}

	const std::string& Sink::GetRegisteredName() const noexcept
	{
		return m_RegisteredName;
	}

	const std::string& Sink::GetPassName() const noexcept
	{
		return m_PassName;
	}

	const std::string& Sink::GetOutputName() const noexcept
	{
		return m_OutputName;
	}

	void Sink::SetTarget(const std::string& passName, const std::string& outputName)
	{
		{
			if (passName.empty())
			{
				throw RGC_EXCEPTION("Empty output name");
			}
			const bool nameCharsValid = std::all_of(passName.begin(), passName.end(), [](char c) {
				return std::isalnum(c) || c == '_';
			});
			if (passName != "$" && (!nameCharsValid || std::isdigit(passName.front())))
			{
				throw RGC_EXCEPTION("Invalid output name: " + m_RegisteredName);
			}
			m_PassName = passName;
		}
		{
			if (outputName.empty())
			{
				throw RGC_EXCEPTION("Empty output name");
			}
			const bool nameCharsValid = std::all_of(outputName.begin(), outputName.end(), [](char c) {
				return std::isalnum(c) || c == '_';
			});
			if (!nameCharsValid || std::isdigit(outputName.front()))
			{
				throw RGC_EXCEPTION("Invalid output name: " + m_RegisteredName);
			}
			m_OutputName = outputName;
		}
	}
}