#include "GraphicsAPI.h"


void GVM::GraphicsAPI::Present() {



    
}


void GVM::GraphicsAPI::ClearRenderTargets(const IRenderTargetView** renderTargets, int32_t numRenderTargets, FColor color) {}
void GVM::GraphicsAPI::ClearRenderTarget(const IRenderTargetView* renderTarget, FColor color) {}
void GVM::GraphicsAPI::Clear(const IDepthStencilView* septhStencil, float depth, int8_t stencil) {}
void GVM::GraphicsAPI::ClearState() {}




void GVM::GraphicsAPI::DrawIndexedPrimitives(int32_t baseVertex, int32_t minVertexIndex, int32_t numVertices, int32_t startIndex, int32_t primitiveCount) {}
void GVM::GraphicsAPI::DrawInstancedPrimitives(int32_t baseVertex, int32_t minVertexIndex, int32_t numVertices, int32_t startIndex, int32_t primitiveCount, int32_t instanceCount) {}
void GVM::GraphicsAPI::DrawPrimitives(int32_t vertexStart, int32_t primitiveCount) {}
void GVM::GraphicsAPI::Dispatch(size_t x, size_t y, size_t z) {}






void GVM::GraphicsAPI::SetViewports(const ViewportDesc viewport[], uint8_t num, uint8_t offset) {}
void GVM::GraphicsAPI::SetViewport(const ViewportDesc& viewport, uint8_t slot) {}
void GVM::GraphicsAPI::SetBlendStates(const BlendDesc& blendState, FColor blendFactor) {}
void GVM::GraphicsAPI::SetDepthStencilState(const DepthStencilDesc& depthStencilState) {}
void GVM::GraphicsAPI::SetRasterizerState(const RasterizerStateDesc& rasterizerState) {}
void GVM::GraphicsAPI::SetSamplers(const SamplerStateDesc samplers[], uint8_t num, uint8_t offset) {}
void GVM::GraphicsAPI::SetSampler(const SamplerStateDesc& sampler, uint8_t slot, EShaderType shader) {}
void GVM::GraphicsAPI::SetupVertexBuffer(const VertexBufferBinding& bindings) {}
void GVM::GraphicsAPI::SetupIndexBuffer(const IIndexBufferView* indices) {}
void GVM::GraphicsAPI::SetupMeshBuffers(const Mesh& bindings) {}
void GVM::GraphicsAPI::SetupTextures(const IResourceView** textures, uint8_t num, uint8_t offset) {}
void GVM::GraphicsAPI::SetupRenderTargets(const RenderTargetDesc* renderTargets, int32_t numRenderTargets, uint8_t offset, IDepthStencilView* depthStencilBuffer) {}
void GVM::GraphicsAPI::SetupRenderTargets(const RenderTargetDesc renderTarget, int32_t slot, IDepthStencilView* depthStencilBuffer) {}
void GVM::GraphicsAPI::SetupShader(const IShader* shader) {}
void GVM::GraphicsAPI::SetupConstBuffers(IConstBuffer* constBuffers[], uint8_t num, uint8_t offset) {}
void GVM::GraphicsAPI::SetupConstBuffer(IConstBuffer* constBuffer, uint8_t slot) {}
void GVM::GraphicsAPI::ClearPipelineState() {}
GVM::IResource* GVM::GraphicsAPI::CreateBuffer(const BufferDesc& description) {}
GVM::IResource* GVM::GraphicsAPI::CreateTexture1D(const Texture1DDesc& description) {}
GVM::IResource* GVM::GraphicsAPI::CreateTexture2D(const Texture2DDesc& description) {}
GVM::IResource* GVM::GraphicsAPI::CreateTexture3D(const Texture3DDesc& description) {}
GVM::IResourceView* GVM::GraphicsAPI::CreateResourceView(const ShaderResourceViewDesc& description) {}
GVM::IResourceView* GVM::GraphicsAPI::CreateRtView(const RenderTargetViewDesc& description) {}
GVM::IVertexBuffer* GVM::GraphicsAPI::CreateVertexBuffer(const VertexBufferDesc& description) {}
GVM::IIndexBuffer* GVM::GraphicsAPI::CreateIndexBuffer(const IndexBufferDesc& description) {}
GVM::IConstBuffer* GVM::GraphicsAPI::CreateConstBuffer(const ConstBufferDesc& description) {}
void GVM::GraphicsAPI::AddDisposeResource(const IResource* texture) {}
void GVM::GraphicsAPI::SetResourceData(const IResource* resource, uint16_t dstSubresource, const UBox rect, const void* pSrcData, int32_t srcRowPitch, int32_t srcDepthPitch) {}
void GVM::GraphicsAPI::SetVertexBufferData(const IVertexBuffer* vertexBuffer, const void* pSrcData, uint32_t dataLength, int32_t srcRowPitch, int32_t srcDepthPitch) {}
void GVM::GraphicsAPI::SetIndexBufferData(const IIndexBuffer* buffer, const void* pSrcData, uint32_t dataLength, int32_t srcRowPitch, int32_t srcDepthPitch) {}
void GVM::GraphicsAPI::SetConstBufferData(IConstBuffer* constBuffer, void* data) {}
GVM::IInputLayout* GVM::GraphicsAPI::CreateInputLayout(const InputAssemblerDeclarationDesc& desc) {}
GVM::IInputLayout* GVM::GraphicsAPI::CreateShader(const InputAssemblerDeclarationDesc& desc, EShaderType type) {}
void GVM::GraphicsAPI::BeginEvent(const char* name) {}
void GVM::GraphicsAPI::EndEvent() {}
void GVM::GraphicsAPI::SetMarker(const char* name) {}
