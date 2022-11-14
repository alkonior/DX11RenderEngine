#pragma once
#include "IRenderer.h"
#include "D3D11Shaders.h"
#include "winHandler.h"
#include "GraphicsExceptions/DxgiInfoManager.h"
#include "GVMBaseStructures.h"

namespace Renderer
{
    
struct DX11ShadersCompiler  
{
private:
    HRESULT hr;
    DxgiInfoManager* infoManager;
public:
    DX11ShadersCompiler();
    ~DX11ShadersCompiler() ;
    D3D11PixelShader* CompilePixelShader(
        const ShaderCompileData& shaderData) ;
    D3D11ComputeShader* CompileComputeShader(
        const ShaderCompileData& shaderData) ;
    D3D11GeometryShader* CompileGeometryShader(
        const ShaderCompileData& shaderData) ;
    D3D11VertexShader* CompileVertexShader(
        const ShaderCompileData& shaderData) ;
    
    GVM::InputAssemblerDeclarationDesc ToGVM(const void* inputLayout, uint32_t inputLayoutSize);
};

}