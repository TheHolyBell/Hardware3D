#include "RenderGraphCompileException.h"
#include <sstream>

namespace RenderGraph
{
	RenderGraphCompileException::RenderGraphCompileException(const std::string& message, int line, const char* file) noexcept
		: ChiliException(line, file), m_Message(message)
	{
	}

	const char* RenderGraphCompileException::what() const noexcept
	{
		std::ostringstream oss;
		oss << ChiliException::what() << std::endl;
		oss << "[Message]" << std::endl;
		oss << m_Message;
		m_WhatBuffer = oss.str();
		return m_WhatBuffer.c_str();
	}

	const char* RenderGraphCompileException::GetType() const noexcept
	{
		return "Render Graphics Compile Exception";
	}

	const std::string& RenderGraphCompileException::GetMessage() const noexcept
	{
		return m_Message;
	}
}
