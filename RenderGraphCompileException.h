#pragma once
#include "ChiliException.h"

#define RGC_EXCEPTION(message) RenderGraphCompileException ( (message), __LINE__, __FILE__ )


namespace RenderGraph
{
	class RenderGraphCompileException : public ChiliException
	{
	public:
		RenderGraphCompileException(const std::string& message, int line, const char* file) noexcept;

		virtual const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		const std::string& GetMessage() const noexcept;
	private:
		std::string m_Message;
	};
}