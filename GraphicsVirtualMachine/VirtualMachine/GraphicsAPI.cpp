#include "GraphicsAPI.h"

using namespace GVM;

GraphicsApi::GraphicsApi(const RenderDeviceInitParams& initParams, bool debugMode):
graphicsMachine(initParams, debugMode)
{
    
    
}

void GraphicsApi::Present() {



    
}


void GraphicsApi::ClearRenderTargets(const IRenderTargetView** renderTargets, int32_t numRenderTargets, FColor color) {}
void GraphicsApi::ClearRenderTarget(const IRenderTargetView* renderTarget, FColor color) {}
void GraphicsApi::Clear(const IDepthStencilView* septhStencil, float depth, int8_t stencil) {}
void GraphicsApi::ClearState()
{
    //ps.VS = ps.PS = ps.CS = ps.GS = ps.HS = ps.DS = nullptr;
    //
    //ps.DrawCallsNum = 0;
    //ps.primitiveType = EPrimitiveTopology::PRIMITIVE_TOPOLOGY_UNDEFINED;
    //ps.rasterizerState = RasterizerStateDesc::Default;
    //ps.depthStencilState  = DepthStencilStateDesc::Default;
    //ps.vertexDeclaration = nullptr;
    //ps.DepthStencilBuffer = nullptr;
    //ps.mesh = Mesh::VoidMesh;
    //ps.blendDesc = CoreBlendDesc();
//
    //ps.renderTargetsNum = 0;
    //ps.RenderTargets = {};
    //ps.samplersNum = 0;
    //ps.Samplers = {};
    //ps.viewportsNum = 0;
    //ps.Viewports = {};
    //ps.constBuffersNum = 0;
    //ps.ConstBuffers = {};
    //ps.texturesNum = 0;
    //ps.Textures = {};
    ps = PipelineSnapshot();
    
    wasPSUpdated = true;
}




void GraphicsApi::DrawIndexedPrimitives(int32_t baseVertex, int32_t minVertexIndex, int32_t numVertices, int32_t startIndex, int32_t primitiveCount)
{
    if (wasPSUpdated)
    {
        graphicsMachine.PushPSC(ps);
        wasPSUpdated = false;
    }
    graphicsMachine.PushDrawCall(DrawCall{EDrawCallType::DRAW_INDEXED, {baseVertex,minVertexIndex,numVertices,startIndex,primitiveCount}});
}

void GraphicsApi::DrawInstancedPrimitives(int32_t baseVertex, int32_t minVertexIndex, int32_t numVertices, int32_t startIndex, int32_t primitiveCount, int32_t instanceCount)
{
    if (wasPSUpdated)
    {
        graphicsMachine.PushPSC(ps);
        wasPSUpdated = false;
    }
    graphicsMachine.PushDrawCall(DrawCall{EDrawCallType::DRAW_INSTANCED, {baseVertex,minVertexIndex,numVertices,startIndex,primitiveCount}});
}

void GraphicsApi::DrawPrimitives(int32_t vertexStart, int32_t primitiveCount)
{
    if (wasPSUpdated)
    {
        graphicsMachine.PushPSC(ps);
        wasPSUpdated = false;
    }
    graphicsMachine.PushDrawCall(DrawCall{EDrawCallType::DRAW, {vertexStart, primitiveCount,0,0,0}});
}

void GraphicsApi::Dispatch(size_t x, size_t y, size_t z)
{
    if (wasPSUpdated)
    {
        graphicsMachine.PushPSC(ps);
        wasPSUpdated = false;
    }
    graphicsMachine.PushDrawCall(DrawCall{EDrawCallType::DISPATCH, {x,y,z,0,0}});
}






void GraphicsApi::SetupViewports(const ViewportDesc viewport[], uint8_t num, uint8_t offset)
{
    wasPSUpdated = true;
    for (int i = 0; i < num; i++)
    {
        ps.Viewports[offset+i] = viewport[i];
    }
    ps.viewportsNum = std::max<uint8_t>(ps.viewportsNum, offset+num);
}
void GraphicsApi::SetupViewport(const ViewportDesc& viewport, uint8_t slot)
{
    wasPSUpdated = true;
    ps.Viewports[slot] = viewport;
    ps.viewportsNum = std::max<uint8_t>(ps.viewportsNum, slot);
}
void GraphicsApi::SetupCoreBlendState(const CoreBlendDesc& blendState)
{
    wasPSUpdated = true;
    ps.blendDesc = blendState;
}
void GraphicsApi::SetupDepthStencilState(const DepthStencilStateDesc& depthStencilState)
{
    wasPSUpdated = true;
    ps.depthStencilState = depthStencilState;
}
void GraphicsApi::SetupRasterizerState(const RasterizerStateDesc& rasterizerState)
{
    wasPSUpdated = true;
    ps.rasterizerState = rasterizerState;
}
void GraphicsApi::SetupSamplers(const SamplerStateDesc samplers[], uint8_t num, uint8_t offset)
{
    wasPSUpdated = true;
    for (int i = 0; i < num; i++)
    {
        ps.Samplers[offset+i] = samplers[i];
    }
    ps.samplersNum = std::max<uint8_t>(ps.samplersNum, offset+num);
}

void GraphicsApi::SetupSampler(const SamplerStateDesc& sampler, uint8_t slot)
{
    wasPSUpdated = true;
    ps.Samplers[slot] = sampler;
    ps.samplersNum = std::max<uint8_t>(ps.samplersNum, slot);
}
void GraphicsApi::SetupVertexBuffer(const VertexBufferBinding& bindings)
{
    wasPSUpdated = true;
    ps.mesh.vertexBuffer = bindings;
}
void GraphicsApi::SetupIndexBuffer(const IIndexBufferView* indices)
{
    wasPSUpdated = true;
    ps.mesh.indexBuffer = (IIndexBufferView*)indices;
}
void GraphicsApi::SetupMeshBuffers(const Mesh& bindings)
{
    ps.mesh = bindings;
}
void GraphicsApi::SetupTextures(const IResourceView* textures[], uint8_t num, uint8_t offset)
{
    wasPSUpdated = true;
    for (int i = 0; i < num; i++)
    {
        ps.Textures[i+offset] = (IResourceView*)textures[i];
    }
    ps.texturesNum =  std::max<uint8_t>(ps.texturesNum, offset+num);
    
}
void GraphicsApi::SetupRenderTargets(const RenderTargetDesc renderTargets[], int32_t num, uint8_t offset,
    IDepthStencilView* depthStencilBuffer)
{
    wasPSUpdated = true;
    ps.DepthStencilBuffer = depthStencilBuffer;
    
    for (int i = 0; i < num; i++)
    {
        ps.RenderTargets[i+offset] = renderTargets[i];
    }
    ps.renderTargetsNum =  std::max<uint8_t>(ps.renderTargetsNum, offset+num);
}
void GraphicsApi::SetupRenderTarget(const RenderTargetDesc renderTarget, int32_t slot, IDepthStencilView* depthStencilBuffer)
{
    wasPSUpdated = true;
    ps.RenderTargets[slot] = renderTarget;
    ps.renderTargetsNum =  std::max<uint8_t>(ps.renderTargetsNum, slot);
}

void GraphicsApi::SetupDepthStencilBuffer( IDepthStencilView* depthStencilBuffer)
{
    wasPSUpdated = true;
    ps.DepthStencilBuffer = depthStencilBuffer;
}

void GraphicsApi::SetupShader(const IShader* shader, EShaderType type)
{
    wasPSUpdated = true;
    switch (type)
    {
        case EShaderType::ComputeShader  : ps.CS = (IShader*)shader; break;
        case EShaderType::DomainShader   : ps.DS = (IShader*)shader; break;
        case EShaderType::GeometryShader : ps.GS = (IShader*)shader; break;
        case EShaderType::HullShader     : ps.HS = (IShader*)shader; break;
        case EShaderType::PixelShader    : ps.PS = (IShader*)shader; break;
        case EShaderType::VertexShader   : ps.VS = (IShader*)shader; break;
    }
}
void GraphicsApi::SetupConstBuffers(IConstBuffer* constBuffers[], uint8_t num, uint8_t offset)
{
    wasPSUpdated = true;
    for (int i = 0; i<num; i++)
    {
        ps.ConstBuffers[offset+i] = constBuffers[i];
    }
    ps.constBuffersNum = std::max<uint8_t>(ps.constBuffersNum, offset+num);
}
void GraphicsApi::SetupConstBuffer(IConstBuffer* constBuffer, uint8_t slot)
{
    wasPSUpdated = true;
    ps.ConstBuffers[slot] = constBuffer;
    ps.constBuffersNum = std::max<uint8_t>(ps.constBuffersNum, slot);
    
}

void GraphicsApi::SetupInputLayout(IInputLayout* layout)
{
    wasPSUpdated = true;
    ps.vertexDeclaration = layout;
}

IResource* GraphicsApi::CreateBuffer(const BufferResourceDesc& description)
{
    return graphicsMachine.resourcesManager.CreateBuffer(description);
}

IResource* GraphicsApi::CreateTexture(const TextureResourceDesc& description)
{
    return graphicsMachine.resourcesManager.CreateTexture(description);
}

IResource* GraphicsApi::CreateTexture1D(const Texture1DResourceDesc& description)
{
    return graphicsMachine.resourcesManager.CreateTexture1D(description);
}

IResource* GraphicsApi::CreateTexture2D(const Texture2DResourceDesc& description)
{
    return graphicsMachine.resourcesManager.CreateTexture2D(description);
}

IResource* GraphicsApi::CreateTexture3D(const Texture3DResourceDesc& description)
{
    return graphicsMachine.resourcesManager.CreateTexture3D(description);
}

IResource* GraphicsApi::CreateTextureCube(const TextureCubeResourceDesc& description)
{
    return graphicsMachine.resourcesManager.CreateTextureCube(description);
}

IResourceView* GraphicsApi::CreateResourceView(const ShaderResourceViewDesc& description)
{
    return graphicsMachine.resourcesManager.CreateShaderResourceView(description);
}

IResourceView* GraphicsApi::CreateRtView(const RenderTargetViewDesc& description)
{
    return graphicsMachine.resourcesManager.CreateRenderTargetView(description);
}

IVertexBuffer* GraphicsApi::CreateVertexBuffer(const VertexBufferDesc& description)
{
    return graphicsMachine.resourcesManager.CreateConstBufferView(description);
}

IIndexBuffer* GraphicsApi::CreateIndexBuffer(const IndexBufferDesc& description)
{
    return graphicsMachine.resourcesManager.CreateRenderTargetView(description);
}

IConstBuffer* GraphicsApi::CreateConstBuffer(const ConstBufferDesc& description)
{
}

IVertexBufferView* GraphicsApi::CreateVertexBufferView(const VertexBufferViewDesc& description)
{
}

IIndexBufferView* GraphicsApi::CreateIndexBufferView(const IndexBufferViewDesc& description)
{
}

IConstBufferView* GraphicsApi::CreateConstBufferView(const ConstBufferViewDesc& description)
{
}

void GraphicsApi::AddDisposeResource(const IResource* texture)
{
}

void GraphicsApi::SetResourceData(const IResource* resource, uint16_t dstSubresource, const UBox rect,
    const void* pSrcData, int32_t srcRowPitch, int32_t srcDepthPitch)
{
}

void GraphicsApi::SetVertexBufferData(const IVertexBuffer* vertexBuffer, const void* pSrcData, uint32_t dataLength,
    int32_t srcRowPitch, int32_t srcDepthPitch)
{
}

void GraphicsApi::SetIndexBufferData(const IIndexBuffer* buffer, const void* pSrcData, uint32_t dataLength,
    int32_t srcRowPitch, int32_t srcDepthPitch)
{
}

void GraphicsApi::SetConstBufferData(IConstBuffer* constBuffer, void* data)
{
}

IInputLayout* GraphicsApi::CreateInputLayout(const InputAssemblerDeclarationDesc& desc)
{
}

IInputLayout* GraphicsApi::CreateShader(const InputAssemblerDeclarationDesc& desc, EShaderType type)
{
}
