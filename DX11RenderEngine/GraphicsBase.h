#pragma once
#include "pch.h"
#include "DxgiInfoManager.h"
#include "GraphicsException.h"
#include "SimpleMath.h"



class GraphicsBase {
public:
	GraphicsBase(HWND hWnd, size_t width, size_t height);

	void OnResize(size_t width, size_t height);



#ifdef _DEBUG
	DxgiInfoManager infoManager;
#endif

    wrl::ComPtr<ID3D11Device> pDevice;
	wrl::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;

	size_t width;
	size_t height;

};