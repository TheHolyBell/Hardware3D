#include "ChiliUtil.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

std::vector<std::string> TokenizeQuoted(const std::string& inputString)
{
    std::istringstream _Stream;

    _Stream.str(inputString);

    std::vector<std::string> _Tokens;
    std::string _token;

    while (_Stream >> std::quoted(_token))
        _Tokens.push_back(std::move(_token));

    return _Tokens;
}

std::wstring ToWide(const std::string& narrow)
{
    return std::wstring(narrow.begin(), narrow.end());
}

std::string ToNarrow(const std::wstring& wide)
{
    return std::string(wide.begin(), wide.end());
}

std::vector<std::string> SplitString(const std::string& s, const std::string& delim)
{
    std::vector<std::string> _Strings;
    SplitStringIter(s, delim, std::back_inserter(_Strings));
    return _Strings;
}

bool StringContains(std::string_view haystack, std::string_view needle)
{
    return std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end()) != haystack.end();
}
