#include "ChiliUtil.h"
#include <sstream>
#include <iomanip>

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
