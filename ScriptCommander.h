#pragma once
#include <vector>
#include <string>
#include "ChiliException.h"

class ScriptCommander
{
public:
	class Completion : public ChiliException
	{
	public:
		Completion(const std::string& content) noexcept;

		virtual const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
	private:
		std::string m_Content;
	};
	class Exception : public ChiliException
	{
	public:
		Exception(int line, const char* file, const std::string& script = "", const std::string& message = "") noexcept;

		virtual const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
	private:
		std::string m_Script;
		std::string m_Message;
	};
public:
	ScriptCommander(const std::vector<std::string>& args);
private:
	void Publish(const std::string& path) const;
};