#include "ChiliException.h"
#include <sstream>

ChiliException::ChiliException(int line, const char* file) noexcept
    : m_Line(line), m_File(file)
{
}

const char* ChiliException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << GetOriginString();
    m_WhatBuffer = oss.str();
    return m_WhatBuffer.c_str();
}

const char* ChiliException::GetType() const noexcept
{
    return "Chili Exception";
}

int ChiliException::GetLine() const noexcept
{
    return m_Line;
}

const std::string& ChiliException::GetFile() const noexcept
{
    return m_File;
}

std::string ChiliException::GetOriginString() const noexcept
{
    std::ostringstream oss;
    oss << "[File] " << m_File << std::endl
        << "[Line] " << m_Line;
    return oss.str();
}
