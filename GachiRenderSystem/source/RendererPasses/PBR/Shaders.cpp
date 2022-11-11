#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 2049
#include "../GameFramework/magic_enum.hpp"
#include <iostream>
#include <vector>

#include "../GameFramework/Game.h"
#include "RenderingSystemPBR.h"

#include <d3d11shader.h>
#include <d3dcompiler.h>

#include "VertexSkin.h"
#include "../GameFramework/VertexPositionNormalBinormalTangentColorTex.h"
#include "../GameFramework/VertexPositionColor.h"

using namespace magic_enum::flags::bitwise_operators;

template<typename T>
std::vector<D3D_SHADER_MACRO> GetMacroses(T flags)
{
	std::vector<D3D_SHADER_MACRO> macros;

	constexpr auto& entries = magic_enum::flags::enum_entries<T>();

	for (const std::pair<T, std::string_view>& p : entries)
	{
		if (static_cast<uint32_t>(flags & p.first) > 0) {
			D3D_SHADER_MACRO macro;
			macro.Name = p.second.data();
			macro.Definition = "1";
			
			macros.push_back(macro);
		}
	}

	macros.push_back(D3D_SHADER_MACRO{nullptr, nullptr});
	return macros;
}


template<typename T>
void LoadVertexShaders(std::map<T, ID3D11VertexShader*>&_vertexShaders, const std::wstring& fileName, const std::vector<T>& flags, ID3DBlob*& _signature)
{
	_signature = nullptr;
	ID3DBlob* vertexBC = nullptr;
	ID3DBlob* errorVertexCode = nullptr;

	for(auto& flag : flags)
	{
		auto macros = GetMacroses(flag);
		auto res = D3DCompileFromFile(fileName.c_str(),
			&macros[0],
			nullptr/*Game::Instance->IncludeShaderHandler*/,
			"VSMain", 
			"vs_5_0", 
			D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 
			0,
			&vertexBC,
			&errorVertexCode
		);

		if (FAILED(res)) {
			if (errorVertexCode) {
				char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
				std::cout << compileErrors << std::endl;
			} else {
				std::wcout << L"Missing Shader File: " << fileName << std::endl;;
			}

			return;
		}

		ID3D11VertexShader* vertexShader;
		Game::Instance->Device->CreateVertexShader(
			vertexBC->GetBufferPointer(),
			vertexBC->GetBufferSize(),
			nullptr, &vertexShader);
		
		if (_signature == nullptr) {
			_signature = vertexBC;
		} else {
			vertexBC->Release();
		}

		_vertexShaders.emplace(flag, vertexShader);
	}
}

template<typename T>
void LoadPixelShaders(std::map<T, ID3D11PixelShader*>& _pixelShaders, const std::wstring& fileName, const std::vector<T>& flags)
{
	ID3DBlob* pixelBC = nullptr;
	ID3DBlob* errorVertexCode = nullptr;

	for (auto& flag : flags) {
		auto macros = GetMacroses(flag);
		auto res = D3DCompileFromFile(fileName.c_str(),
			&macros[0],
			nullptr/*Game::Instance->IncludeShaderHandler*/,
			"PSMain",
			"ps_5_0",
			D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&pixelBC,
			&errorVertexCode
		);

		if (FAILED(res)) {
			if (errorVertexCode) {
				char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
				std::cout << compileErrors << std::endl;
			}
			else {
				std::wcout << L"Missing Shader File: " << fileName << std::endl;;
			}

			return;
		}

		ID3D11PixelShader* pixelShader;
		Game::Instance->Device->CreatePixelShader(
			pixelBC->GetBufferPointer(),
			pixelBC->GetBufferSize(),
			nullptr, &pixelShader);

		_pixelShaders.emplace(flag, pixelShader);
	}
}


void RenderingSystemPBR::ReloadShadersOpaque()
{
	std::map<VertexFlagsOpaque, ID3D11VertexShader*>	_vertexShadersOpaq;
	std::map<PixelFlagsOpaque, ID3D11PixelShader*>		_pixelShadersOpaq;

	std::map<VertexFlagsLighting, ID3D11VertexShader*>	_vertexShadersLigh;
	std::map<PixelFlagsLighting, ID3D11PixelShader*>	_pixelShadersLigh;

	ID3DBlob* _signatureOpaq = nullptr;
	ID3DBlob* _signatureLigh = nullptr;

	LoadVertexShaders(_vertexShadersOpaq, L"./Shaders/PBRFillGBuffer.hlsl", vsFlagsOpaque, _signatureOpaq);
	LoadPixelShaders(_pixelShadersOpaq, L"./Shaders/PBRFillGBuffer.hlsl", psFlagsOpaque);
	
	LoadVertexShaders(_vertexShadersLigh, L"./Shaders/PBRLighting.hlsl", vsFlagsLighting, _signatureLigh);
	LoadPixelShaders(_pixelShadersLigh, L"./Shaders/PBRLighting.hlsl", psFlagsLighting);


	SafeRelease(layoutOpaque);
	layoutOpaque = VertexPositionNormalBinormalTangentColorTex::GetLayout(_signatureOpaq);
	SafeRelease(layoutLighting);
	layoutLighting = VertexPositionColor::GetLayout(_signatureLigh);


	std::vector<D3D_SHADER_MACRO> macros;
	macros.push_back(D3D_SHADER_MACRO{ "SKINNED_MESH", "1" });
	macros.push_back(D3D_SHADER_MACRO{ nullptr, nullptr });

	ID3DBlob* vertexBC = nullptr;
	ID3DBlob* errorVertexCode = nullptr;
	auto res = D3DCompileFromFile(L"./Shaders/PBRFillGBuffer.hlsl",
		&macros[0],
		nullptr/*Game::Instance->IncludeShaderHandler*/,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexBC,
		&errorVertexCode
	);

	layoutOpaqueAnimated = VertexSkin::GetLayout(vertexBC);

	for(auto& val : OpaqueVSs) {
		SafeRelease(val.second);
	}
	for(auto& val : OpaquePSs) {
		SafeRelease(val.second);
	}
	for(auto& val : LightingVSs) {
		SafeRelease(val.second);
	}
	for(auto& val : LightingPSs) {
		SafeRelease(val.second);
	}


	OpaqueVSs = _vertexShadersOpaq;
	OpaquePSs = _pixelShadersOpaq;

	LightingVSs = _vertexShadersLigh;
	LightingPSs = _pixelShadersLigh;


	_signatureOpaq->Release();
	_signatureLigh->Release();
}


void RenderingSystemPBR::ReloadShadersToneMap()
{
	std::map<VSToneMapFlags, ID3D11VertexShader*>	vertexShadersTone;
	std::map<PSToneMapFlags, ID3D11PixelShader*>	pixelShadersTone;

	ID3DBlob* _signatureOpaq = nullptr;

	LoadVertexShaders(vertexShadersTone, L"./Shaders/PBRToneMapping.hlsl", vsFlagsTone, _signatureOpaq);
	LoadPixelShaders(pixelShadersTone, L"./Shaders/PBRToneMapping.hlsl", psFlagsTone);

	for (auto& val : ToneVSs) {
		SafeRelease(val.second);
	}
	for (auto& val : TonePSs) {
		SafeRelease(val.second);
	}

	ToneVSs = vertexShadersTone;
	TonePSs = pixelShadersTone;

	SafeRelease(_signatureOpaq);
}

