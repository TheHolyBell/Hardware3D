#pragma once
#include <unordered_map>
#include <string>
#include <Windows.h>

class WindowsMessageMap
{
	WindowsMessageMap();
public:
	static WindowsMessageMap& GetInstance();
	std::string operator()(DWORD msg, LPARAM lp, WPARAM wp) const;
private:
	std::unordered_map<DWORD, std::string> m_Map;
};