#include "ScriptCommander.h"
#include <sstream>
#include <fstream>
#include <filesystem>
#include "json.hpp"
#include "TexturePreprocessor.h"

namespace JSON = nlohmann;
using namespace std::string_literals;

#define SCRIPT_ERROR(msg) Exception(__LINE__, __FILE__, scriptPath, (msg))

ScriptCommander::ScriptCommander(const std::vector<std::string>& args)
{
	if (args.size() >= 2 && args[0] == "--commands")
	{
		const auto scriptPath = args[1];
		std::ifstream script(scriptPath);
		if (!script.is_open())
			throw SCRIPT_ERROR("Unable to open script file");

		JSON::json _Top;
		script >> _Top;

		if (_Top.at("enabled"))
		{
			bool _bAbort = false;
			for (const auto& j : _Top.at("commands"))
			{
				const auto _CommandName = j.at("command").get<std::string>();
				const auto _Params = j.at("params");
				if (_CommandName == "flip-y")
				{
					const auto source = _Params.at("source");
					TexturePreprocessor::FlipYNormalMap(source, _Params.value("dest", source));
					_bAbort = true;
				}
				else if (_CommandName == "flip-y-obj")
				{
					TexturePreprocessor::FlipYAllNormalMapsInObj(_Params.at("source"));
					_bAbort = true;
				}
				else if (_CommandName == "validate-nmap")
				{
					TexturePreprocessor::ValidateNormalMap(_Params.at("source"), _Params.at("min"), _Params.at("max"));
					_bAbort = true;
				}
				else if (_CommandName == "make-stripes")
				{
					TexturePreprocessor::MakeStripes(_Params.at("dest"), _Params.at("size"), _Params.at("stripeWidth"));
					_bAbort = true;
				}
				else if (_CommandName == "publish")
				{
					Publish(_Params.at("dest"));
					_bAbort = true;
				}
				else
				{
					throw SCRIPT_ERROR("Unknown command: "s + _CommandName);
				}
			}
			if (_bAbort)
				throw Completion("Command(s) completed successfully");
		}
	}
}

void ScriptCommander::Publish(const std::string& path) const
{
	namespace fs = std::filesystem;
	fs::create_directory(path);
	// Copy executable
	fs::copy_file(R"(..\x64\Release\hw3d.exe)", path + R"(\hw3d.exe)", fs::copy_options::overwrite_existing);
	// copy assimp ini
	fs::copy_file("imgui_default.ini", path + R"(\imgui_default.ini)", fs::copy_options::overwrite_existing);
	
	// Copy all DLLs
	for (const auto& p : fs::directory_iterator(""))
	{
		if (p.path().extension() == L".dll")
			fs::copy_file(p.path(), path + "\\" + p.path().filename().string(),
				fs::copy_options::overwrite_existing);
	}

	// Copy compiled shaders
	fs::copy("ShaderBins", path + R"(\ShaderBins)", fs::copy_options::overwrite_existing);
	// Copy assets
	fs::copy("Images", path + R"(\Images)", fs::copy_options::overwrite_existing | fs::copy_options::recursive);
	fs::copy("Models", path + R"(\Models)", fs::copy_options::overwrite_existing | fs::copy_options::recursive);
}

ScriptCommander::Completion::Completion(const std::string& content) noexcept
	: ChiliException(69, "@ScriptCommanderAbort"), m_Content(content)
{
	
}

const char* ScriptCommander::Completion::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << "\n\n" << m_Content;
	m_WhatBuffer = oss.str();
	return m_WhatBuffer.c_str();
}

const char* ScriptCommander::Completion::GetType() const noexcept
{
	return "Script Command Completed";
}

ScriptCommander::Exception::Exception(int line, const char* file, const std::string& script, const std::string& message) noexcept
	: ChiliException(69, "@ScriptCommanderAbort"),
	m_Script(script), m_Message(message)
{}

const char* ScriptCommander::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Script File] " << m_Script << std::endl
		<< m_Message;
	m_WhatBuffer = oss.str();
	return m_WhatBuffer.c_str();
}

const char* ScriptCommander::Exception::GetType() const noexcept
{
	return "Script Command Error";
}