#pragma once
#include <vector>
#include <string>
#include <iterator>

std::vector<std::string> TokenizeQuoted(const std::string& inputString);

std::wstring ToWide(const std::string& narrow);

std::string ToNarrow(const std::wstring& wide);

template<typename Iter>
void SplitStringIter(const std::string& s, const std::string& delim, Iter out)
{
	if (delim.empty())
		*out++ = s;
	else
	{
		size_t a = 0, b = s.find(delim);
		for (; b != std::string::npos;
			a = b + delim.length(), b = s.find(delim, a))
		{
			*out++ = std::move(s.substr(a, b - a));
		}
		*out++ = std::move(s.substr(a, s.length() - a));
	}
}

inline std::vector<std::string> SplitString(const std::string& s, const std::string& delim)
{
	std::vector<std::string> _Strings;
	SplitStringIter(s, delim, std::back_inserter(_Strings));
	return _Strings;
}