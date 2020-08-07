#pragma once
#include "ChiliException.h"

class ModelException : public ChiliException
{
public:
	ModelException(int line, const char* file, std::string note) noexcept;

	virtual const char* what() const noexcept override;
	virtual const char* GetType() const noexcept override;

	const std::string& GetNote() const noexcept;
private:
	std::string m_Note;
};