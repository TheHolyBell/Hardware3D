#include "ModelException.h"
#include <sstream>

ModelException::ModelException(int line, const char* file, std::string note) noexcept
	: ChiliException(line, file), m_Note(std::move(note))
{

}

const char* ModelException::what() const noexcept
{
	std::ostringstream oss;
	oss << ChiliException::what() << std::endl
		<< "[Note] " << GetNote();
	m_WhatBuffer = oss.str();
	return m_WhatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "Chili Model Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
	return m_Note;
}