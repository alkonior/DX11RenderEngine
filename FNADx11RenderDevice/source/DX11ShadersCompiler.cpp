#include "DX11ShadersCompiler.h"

#include "GraphicsExceptions/GraphicsException.h"

using namespace Renderer;

DX11ShadersCompiler::DX11ShadersCompiler()
{
    infoManager = new DxgiInfoManager();
}

DX11ShadersCompiler::~DX11ShadersCompiler()
{
    delete infoManager;
}

D3D11PixelShader* DX11ShadersCompiler::CompilePixelShader(const ShaderCompileData& shaderData)
    {
        D3D11PixelShader* result = new D3D11PixelShader();
        std::vector<D3D_SHADER_MACRO> d3ddefines(shaderData.definesSize + 1);
        for (size_t i = 0; i < shaderData.definesSize; i++)
        {
            d3ddefines[i].Definition = shaderData.defines[i].defenition;
            d3ddefines[i].Name = shaderData.defines[i].name;
        }
        d3ddefines[shaderData.definesSize].Definition = NULL;
        d3ddefines[shaderData.definesSize].Name = NULL;

        wrl::ComPtr<ID3D10Blob> pPSData;
        wrl::ComPtr<ID3D10Blob> psErrorBlob;
#if _DEBUG
        bool compiled = false;
        try
        {
            GFX_THROW_INFO(
                D3DCompile(shaderData.shaderData, shaderData.dataSize, NULL,
                    d3ddefines.data(), (ID3DInclude*)shaderData.includes,
                    shaderData.enteryPoint, shaderData.target,
                    shaderData.flags, shaderData.flags << 8u, &pPSData, &psErrorBlob));
            GFX_THROW_INFO(
                device->CreatePixelShader(pPSData->GetBufferPointer(), pPSData->GetBufferSize(), nullptr, &result->
                    pPixelShader));
            compiled = true;
        }
        catch (HrException exe)
        {
            CompileException ce{
                __LINE__, __FILE__,(hr),infoManager->GetMessages(),(char*)psErrorBlob->GetBufferPointer(),
                shaderData.name, "PixelShader", d3ddefines
            };
            throw ce;
        }
        catch (InfoException exe)
        {
            CompileException ce{
                __LINE__, __FILE__,(hr),infoManager->GetMessages(),(char*)psErrorBlob->GetBufferPointer(),
                shaderData.name, "PixelShader", d3ddefines
            };
            throw ce;
        }
#else
        GFX_THROW_INFO(D3DCompile(shaderData, dataSize, NULL, d3ddefines.data(), (ID3DInclude*)includes, enteryPoint, target, flags, flags << 8u, &pPSData, &psErrorBlob));
        GFX_THROW_INFO(device->CreatePixelShader(pPSData->GetBufferPointer(), pPSData->GetBufferSize(), nullptr, &result->pComputeShader));

        GVM::ShaderDesc desc;
        desc.type = GVM::EShaderType::PIXEL_SHADER;
        //desc.name = shaderData.name;
        desc.bytecode = pPSData->GetBufferPointer();
        desc.byteCodeSize = pPSData->GetBufferSize();
        result->testShader = testApi->CreateShader(desc);

#endif
        return result;
    }


    D3D11ComputeShader* DX11ShadersCompiler::CompileComputeShader(const ShaderCompileData& shaderData)
    {
        D3D11ComputeShader* result = new D3D11ComputeShader();
        std::vector<D3D_SHADER_MACRO> d3ddefines(shaderData.definesSize + 1);
        for (size_t i = 0; i < shaderData.definesSize; i++)
        {
            d3ddefines[i].Definition = shaderData.defines[i].defenition;
            d3ddefines[i].Name = shaderData.defines[i].name;
        }
        d3ddefines[shaderData.definesSize].Definition = NULL;
        d3ddefines[shaderData.definesSize].Name = NULL;

        wrl::ComPtr<ID3D10Blob> pPSData;
        wrl::ComPtr<ID3D10Blob> psErrorBlob;
#if _DEBUG
        try
        {
            GFX_THROW_INFO(
                D3DCompile(shaderData.shaderData, shaderData.dataSize, NULL,
                    d3ddefines.data(), (ID3DInclude*)shaderData.includes, shaderData.enteryPoint,
                    shaderData.target, shaderData.flags, shaderData.flags << 8u, &pPSData, &psErrorBlob));
            GFX_THROW_INFO(
                device->CreateComputeShader(pPSData->GetBufferPointer(), pPSData->GetBufferSize(), nullptr, &result->
                    pComputeShader));
        }
        catch (HrException exe)
        {
            CompileException ce{
                __LINE__, __FILE__,(hr),infoManager->GetMessages(),
                (char*)psErrorBlob->GetBufferPointer(),
                shaderData.name, "Compute Shader", d3ddefines
            };
            throw ce;
        }
        catch (InfoException exe)
        {
            CompileException ce{
                __LINE__, __FILE__,(hr),infoManager->GetMessages(),
                (char*)psErrorBlob->GetBufferPointer(),
                shaderData.name, "ComputeShader", d3ddefines
            };
            throw ce;
        }
#else
        GFX_THROW_INFO(
                       D3DCompile(shaderData.shaderData, shaderData.dataSize, NULL,
                           d3ddefines.data(), (ID3DInclude*)shaderData.includes,
                           shaderData.enteryPoint,shaderData.target, shaderData.flags,
                           shaderData.flags << 8u, &pPSData, &psErrorBlob));
        GFX_THROW_INFO(
            device->CreateComputeShader(pPSData->GetBufferPointer(),
                pPSData->GetBufferSize(), nullptr, &result->pComputeShader));

#endif
        return result;
    }

    D3D11GeometryShader* DX11ShadersCompiler::CompileGeometryShader(const ShaderCompileData& shaderData)
    {
        D3D11GeometryShader* result = new D3D11GeometryShader();
        std::vector<D3D_SHADER_MACRO> d3ddefines(shaderData.definesSize + 1);
        for (size_t i = 0; i < shaderData.definesSize; i++)
        {
            d3ddefines[i].Definition = shaderData.defines[i].defenition;
            d3ddefines[i].Name = shaderData.defines[i].name;
        }
        d3ddefines[shaderData.definesSize].Definition = NULL;
        d3ddefines[shaderData.definesSize].Name = NULL;

        wrl::ComPtr<ID3D10Blob> pPSData;
        wrl::ComPtr<ID3D10Blob> psErrorBlob;
#if _DEBUG
        try
        {
            GFX_THROW_INFO(
                D3DCompile(shaderData.shaderData, shaderData.dataSize,
                    NULL, d3ddefines.data(), (ID3DInclude*)shaderData.includes,
                    shaderData.enteryPoint, shaderData.target, shaderData.flags,
                    shaderData.flags << 8u, &pPSData, &psErrorBlob));
            GFX_THROW_INFO(
                device->CreateGeometryShader(pPSData->GetBufferPointer(), pPSData->GetBufferSize(), nullptr, &result->
                    pGeometryShader));
            
        }
        catch (HrException exe)
        {
            CompileException ce{
                __LINE__, __FILE__,(hr),infoManager->GetMessages(),(char*)psErrorBlob->GetBufferPointer(),
                shaderData.name
            };
            throw ce;
        }
        catch (InfoException exe)
        {
            CompileException ce{
                __LINE__, __FILE__,(hr),infoManager->GetMessages(),(char*)psErrorBlob->GetBufferPointer(),
                shaderData.name
            };
            throw ce;
        }
#else
        GFX_THROW_INFO(
                       D3DCompile(shaderData.shaderData, shaderData.dataSize,
                           NULL, d3ddefines.data(), (ID3DInclude*)shaderData.includes,
                           shaderData.enteryPoint, shaderData.target, shaderData.flags,
                           shaderData.flags << 8u, &pPSData, &psErrorBlob));
        GFX_THROW_INFO(
            device->CreateGeometryShader(pPSData->GetBufferPointer(), pPSData->GetBufferSize(), nullptr, &result->
                pGeometryShader));
#endif
        return result;
    }



    D3D11VertexShader* DX11ShadersCompiler::CompileVertexShader(const ShaderCompileData& shaderData, void* inputLayout,
        size_t inputLayoutSize)
    {
        D3D11VertexShader* result = new D3D11VertexShader();

        std::vector<D3D_SHADER_MACRO> d3ddefines(shaderData.definesSize + 1);
        for (size_t i = 0; i < shaderData.definesSize; i++)
        {
            d3ddefines[i].Definition = shaderData.defines[i].defenition;
            d3ddefines[i].Name = shaderData.defines[i].name;
        }
        d3ddefines[shaderData.definesSize].Definition = NULL;
        d3ddefines[shaderData.definesSize].Name = NULL;

        wrl::ComPtr<ID3D10Blob> pVSData;
        wrl::ComPtr<ID3D10Blob> psErrorBlob;

        const D3D11_INPUT_ELEMENT_DESC* d3dInputLayout = (const D3D11_INPUT_ELEMENT_DESC*)inputLayout;
        //{
        //		{"Position",  0, DXGI_FORMAT_R32G32_FLOAT,  0,                           0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        //		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,  0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        //};
        #if _DEBUG
        try
        {
            GFX_THROW_INFO(
                D3DCompile(shaderData.shaderData, shaderData.dataSize, NULL,
                    d3ddefines.data(), (ID3DInclude*)shaderData.includes,
                    shaderData.enteryPoint, shaderData.target,
                    shaderData.flags, shaderData.flags << 8u, &pVSData, &psErrorBlob));

            //todo InputLayout
        }
        catch (HrException exe)
        {
            if (psErrorBlob != nullptr)
            {
                CompileException ce{
                    __LINE__, __FILE__,(hr),infoManager->GetMessages(),
                    (char*)psErrorBlob->GetBufferPointer(),
                    shaderData.name, "VertexShader", d3ddefines
                };
                throw ce;
            }
            throw;
        }
        catch (InfoException exe)
        {
            if (psErrorBlob != nullptr)
            {
                CompileException ce{
                    __LINE__, __FILE__,(hr),infoManager->GetMessages(),
                    (char*)psErrorBlob->GetBufferPointer(),
                    shaderData.name, "VertexShader", d3ddefines
                };
                throw ce;
            }
            throw;
        }
#else
        GFX_THROW_INFO(D3DCompile(shaderData, dataSize, NULL, d3ddefines.data(), (ID3DInclude*)includes, enteryPoint, target, flags, flags << 8u, &pVSData, &psErrorBlob));

        GFX_THROW_INFO(device->CreateVertexShader(pVSData->GetBufferPointer(), pVSData->GetBufferSize(), nullptr, &result->pVertexShader));
        GFX_THROW_INFO(device->CreateInputLayout(
            d3dInputLayout,
            (UINT)inputLayoutSize,
            pVSData->GetBufferPointer(),
            pVSData->GetBufferSize(),
            &result->pInputLayout));
#endif
        return result;
    }