#include "GraphicsAPI.h"


void GVM::GraphicsAPI::Present() {
    //todo Run VM
}


void GVM::GraphicsAPI::ClearRenderTarget(const IRenderTarget** renderTargets, int32_t numRenderTargets, FColor color) {
    
}
void GVM::GraphicsAPI::Clear(float depth, int8_t stencil) {
    
}
void GVM::GraphicsAPI::ResetBackbuffer(const IRenderDeviceInitParams* initParams) {
    
}
void GVM::GraphicsAPI::DrawIndexedPrimitives(int32_t baseVertex, int32_t minVertexIndex, int32_t numVertices, int32_t startIndex, int32_t primitiveCount) {
    
}
void GVM::GraphicsAPI::DrawInstancedPrimitives(int32_t baseVertex, int32_t minVertexIndex, int32_t numVertices, int32_t startIndex, int32_t primitiveCount, int32_t instanceCount) {
    
}
void GVM::GraphicsAPI::DrawPrimitives(int32_t vertexStart, int32_t primitiveCount) {
    
}
void GVM::GraphicsAPI::Dispatch(size_t x, size_t y, size_t z) {
    
}
const GVM::IViewport* GVM::GraphicsAPI::CreateViewport(const ViewportDesc* viewportDesc) {
    
}
const GVM::IScissorRect* GVM::GraphicsAPI::CreateScissorRect(const Rect* scissor) {
    
}
const GVM::IBlendState* GVM::GraphicsAPI::CreateBlendState(const BlendDesc* blendStates) {
    
}
const GVM::IDepthStencilState* GVM::GraphicsAPI::CreateDepthStencilState(const DepthStencilStateDesc* depthStencilState) {
    
}
const GVM::IRasterizerState* GVM::GraphicsAPI::CreateRasterizerStat(const RasterizerStateDesc* rasterizerState) {
    
}
const GVM::ISamplerState* GVM::GraphicsAPI::CreateSampler(const SamplerStateDesc* sampler) {
    
}
GVM::IShader* GVM::GraphicsAPI::CreateShader(const ShaderDesc& shaderDesc, EShaderType type) {
    
}
void GVM::GraphicsAPI::AddDisposeViewport(const IViewport* viewportDesc) {
    
}
void GVM::GraphicsAPI::AddDisposeScissorRect(const IScissorRect* scissor) {
    
}
void GVM::GraphicsAPI::AddDisposeBlendState(const IBlendState* blendStates) {
    
}
void GVM::GraphicsAPI::AddDisposeDepthStencilState(const IDepthStencilState* depthStencilState) {
    
}
void GVM::GraphicsAPI::AddDisposeRasterizerStat(const IRasterizerState* rasterizerState) {
    
}
void GVM::GraphicsAPI::AddDisposeSampler(const ISamplerState* sampler) {
    
}
void GVM::GraphicsAPI::AddDisposeShader(IShader* shader) {
    
}
void GVM::GraphicsAPI::SetViewports(const IViewport** viewport, uint8_t num) {
    
}
void GVM::GraphicsAPI::SetScissorRect(const IScissorRect** scissors, uint8_t num) {
    
}
void GVM::GraphicsAPI::SetBlendStates(const IBlendState* blendState, FColor blendFactor, uint32_t sampleMask) {
    
}
void GVM::GraphicsAPI::SetDepthStencilState(const IDepthStencilState* depthStencilState) {
    
}
void GVM::GraphicsAPI::SetRasterizerState(const IRasterizerState* rasterizerState) {
    
}
void GVM::GraphicsAPI::SetSamplers(const ISamplerState** sampler, uint8_t num, EShaderType shader) {
    
}
void GVM::GraphicsAPI::SetPipelineState(const IPipelineState pipelineState) {}
void GVM::GraphicsAPI::SetupVertexBuffer(const VertexBufferBinding& bindings) {}
void GVM::GraphicsAPI::SetupIndexBuffer(const IIndexBufferView* indices) {}
void GVM::GraphicsAPI::SetupMeshBuffers(const Mesh& bindings) {}
void GVM::GraphicsAPI::SetupTextures(const IResourceView* texture, uint8_t num, EShaderType shader) {}
void GVM::GraphicsAPI::SetupVertexBuffer(const IVertexBuffer* bindings) {}
void GVM::GraphicsAPI::SetupRenderTargets(const IRenderTarget** renderTargets, int32_t numRenderTargets, IResourceView* depthStencilBuffer, bool isCompute) {}
void GVM::GraphicsAPI::SetupShader(const IShader* shader, EShaderType type) {}
void GVM::GraphicsAPI::SetupConstBuffers(IConstBuffer** constBuffers, size_t num) {}
void GVM::GraphicsAPI::ClearPipelineState() {}
GVM::IResource* GVM::GraphicsAPI::CreateBuffer(const BufferDesc& description) {}
GVM::IResource* GVM::GraphicsAPI::CreateTexture1D(const Texture1DDesc& description) {}
GVM::IResource* GVM::GraphicsAPI::CreateTexture2D(const Texture2DDesc& description) {}
GVM::IResource* GVM::GraphicsAPI::CreateTexture3D(const Texture3DDesc& description) {}
GVM::IResourceView* GVM::GraphicsAPI::CreateResourceView(const IResource* resource, const ResourceViewDesc& description) {}
GVM::IVertexBuffer* GVM::GraphicsAPI::CreateVertexBuffer(const VertexBufferDesc& description) {}
GVM::IIndexBuffer* GVM::GraphicsAPI::CreateIndexBuffer(const IndexBufferDesc& description) {}
GVM::IResource* GVM::GraphicsAPI::ResizeBuffer(const IResource* texture, const BufferDesc& description) {}
GVM::IResource* GVM::GraphicsAPI::ResizeTexture1D(const IResource* texture, const Texture1DDesc& description) {}
GVM::IResource* GVM::GraphicsAPI::ResizeTexture2D(const IResource* texture, const Texture2DDesc& description) {}
GVM::IResource* GVM::GraphicsAPI::ResizeTexture3D(const IResource* texture, const Texture3DDesc& description) {}
void GVM::GraphicsAPI::AddDisposeResource(const IResource* texture) {}
void GVM::GraphicsAPI::SetResourceData(const IResource* resource, uint16_t dstSubresource, const UBox rect, const void* pSrcData, int32_t srcRowPitch, int32_t srcDepthPitch) {}
void GVM::GraphicsAPI::SetVertexBufferData(const IVertexBuffer* vertexBuffer, const void* pSrcData, uint32_t dataLength, int32_t srcRowPitch, int32_t srcDepthPitch) {}
void GVM::GraphicsAPI::SetIndexBufferData(const IIndexBuffer* buffer, const void* pSrcData, uint32_t dataLength, int32_t srcRowPitch, int32_t srcDepthPitch) {}
void GVM::GraphicsAPI::SetConstBufferData(IConstBuffer* constBuffer, void* data) {}
void GVM::GraphicsAPI::BeginEvent(const char* name) {}
void GVM::GraphicsAPI::EndEvent() {}
void GVM::GraphicsAPI::SetMarker(const char* name) {}
