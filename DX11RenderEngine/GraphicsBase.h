#pragma once
#include "pch.h"
#include "D3D11Renderer.h"
#include "Transform.h"
#include "SimpleMath.h"


class GraphicsBase {

static Renderer::PresentationParameters GenParams(HWND hWnd, size_t width, size_t height);

public:
	GraphicsBase(HWND hWnd, size_t width, size_t height);
	void OnResize(size_t width, size_t height);


	Renderer::D3D11Renderer renderer;




#ifdef _DEBUG
	//DxgiInfoManager infoManager;
#endif

    //wrl::ComPtr<ID3D11Device> pDevice;
	//wrl::ComPtr<ID3D11DeviceContext> pContext;
	//Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	//Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	//Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;

	size_t width;
	size_t height;


	Transform camera;
	matrix cameraProjection;


	void SetCameraPosition(Transform position);
	void SetCameraProjection(matrix projection);

};