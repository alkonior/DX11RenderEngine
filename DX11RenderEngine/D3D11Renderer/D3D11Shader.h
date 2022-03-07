#pragma once
#include "pch.h"
#include "Utils.h"


namespace Renderer {

enum D3D11ShaderType {
	VERTEX_SHADER,
	PIXEL_SHADER
};
//
//struct D3D11Shader{
//	D3D11ShaderType type;
//
//	union {
//		struct{
//
//			wrl::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
//			wrl::ComPtr<ID3D10Blob> pPSData = nullptr;
//
//		} pixelShader;
//		struct{
//
//			wrl::ComPtr<ID3D11VertexShader> pVertexShader = nullptr;
//			wrl::ComPtr<ID3D11InputLayout> pInputLayout = nullptr;
//			wrl::ComPtr<ID3D10Blob> pVSData = nullptr;
//
//		} vertexShader;
//	};
//
//};

};