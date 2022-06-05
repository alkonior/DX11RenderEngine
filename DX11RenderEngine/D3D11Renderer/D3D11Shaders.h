#pragma once
#include "pch.h"
#include "IRenderer/Utils.h"

namespace Renderer {

struct D3D11VertexShader: public VertexShader {
	GVM::Shader* testShader;
	GVM::InputLayout* testIL;
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;

};

struct  D3D11PixelShader: public PixelShader {
	GVM::Shader* testShader;
	wrl::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
};

struct  D3D11ComputeShader: public ComputeShader {
	GVM::Shader* testShader;
	wrl::ComPtr<ID3D11ComputeShader> pComputeShader = nullptr;
};

struct  D3D11GeometryShader: public GeometryShader {
	GVM::Shader* testShader;
	wrl::ComPtr<ID3D11GeometryShader> pGeometryShader = nullptr;
};


};

