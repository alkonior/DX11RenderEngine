#pragma once
#include "pch.h"



class GraphicsBase {
public:
	GraphicsBase() :gfx(*this) {};
    wrl::ComPtr<ID3D11Device> pDevice;
	wrl::ComPtr<ID3D11DeviceContext> pContext;
#ifdef _DEBUG
	GraphicsBase& gfx;
	DxgiInfoManager infoManager;
#endif
	
};