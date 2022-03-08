#pragma once
#include "pch.h"
#include "IRenderer/Utils.h"

namespace Renderer {

struct D3D11VertexShader: public VertexShader {
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;

};

struct  D3D11PixelShader: public PixelShader {
	wrl::ComPtr<ID3D11PixelShader> pComputeShader = nullptr;
};

struct  D3D11ComputeShader: public ComputeShader {
	wrl::ComPtr<ID3D11ComputeShader> pComputeShader = nullptr;
};

struct  D3D11GeometryShader: public GeometryShader {
	wrl::ComPtr<ID3D11GeometryShader> pGeometryShader = nullptr;
};


};

