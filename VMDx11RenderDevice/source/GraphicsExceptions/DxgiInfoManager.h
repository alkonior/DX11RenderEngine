#pragma once
#include "winHandler.h"

namespace GVM {

class DxgiInfoManager
{
public:
	DxgiInfoManager();
	~DxgiInfoManager() = default;
	DxgiInfoManager( const DxgiInfoManager& ) = delete;
	DxgiInfoManager& operator=( const DxgiInfoManager& ) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long next = 0u;
	wrl::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
};


}