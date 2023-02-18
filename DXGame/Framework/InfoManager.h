#pragma once
#include "DxWin.h"
#include <vector>
#include <string>

class InfoManager
{
public:
	InfoManager();
	~InfoManager();
	InfoManager(const InfoManager&) = delete;
	InfoManager& operator=(const InfoManager&) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long next = 0u;
	struct IDXGIInfoQueue* pDxgiInfoQueue = nullptr;
};