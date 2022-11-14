#pragma once
//#include "winHandler.h"
#include "IRenderer/Utils.h"
#include "GVMBaseStructures.h"

namespace Renderer {

struct D3D11VertexShader: public VertexShader {
	GVM::Shader* testShader;
	GVM::InputLayout* testIL;
	void* data;
	size_t dataSize;
	//wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	//wrl::ComPtr<ID3D11InputLayout> pInputLayout;

};

struct  D3D11PixelShader: public PixelShader {
	GVM::Shader* testShader;
	void* data;
	size_t dataSize;
	
	//wrl::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
};

struct  D3D11ComputeShader: public ComputeShader {
	GVM::Shader* testShader;
	void* data;
	size_t dataSize;
	//wrl::ComPtr<ID3D11ComputeShader> pComputeShader = nullptr;
};

struct  D3D11GeometryShader: public GeometryShader {
	GVM::Shader* testShader;
	void* data;
	size_t dataSize;
	//wrl::ComPtr<ID3D11GeometryShader> pGeometryShader = nullptr;
};


};

