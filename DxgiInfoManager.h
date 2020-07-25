#pragma once
#include "ChiliWin.h"
#include <vector>
#include <string>

class DxgiInfoManager
{
public:
	DxgiInfoManager();
	~DxgiInfoManager();

	DxgiInfoManager(const DxgiInfoManager& rhs) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager& rhs) = delete;

	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long m_Next = 0u;
	struct IDXGIInfoQueue* m_pDxgiInfoQueue = nullptr;
};