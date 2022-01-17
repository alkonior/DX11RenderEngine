#pragma once
#include "pch.h"
#include "Renderer.h"
#include "D3D11Texture.h"
//#include "D3D11Shader.h"
#include "D3D11Buffer.h"
#include "D3D11VertexBuffer.h"
#include "D3D11RenderBuffer.h"
#include "D3D11Shaders.h"



#include "DxgiInfoManager.h"
#include "GraphicsException.h"
#include <thread>
namespace Renderer{


class D3D11Renderer : public IRenderer {
public:
	/* Persistent D3D11 Objects */
	wrl::ComPtr<ID3D11Device> device;
	wrl::ComPtr<ID3D11DeviceContext> context;
	wrl::ComPtr<IDXGISwapChain> swapchain;

private:
	wrl::ComPtr<IDXGIAdapter> adapter;
	wrl::ComPtr<IDXGIFactory> factory;
	D3D_FEATURE_LEVEL featureLevel;
	//wrl::ComPtr<ID3DUserDefinedAnnotation> annotation;
	//SDL_mutex* ctxLock;

	/* Capabilities */
	uint8_t debugMode;
	//uint32_t supportsDxt1;
	//uint32_t supportsS3tc;
	//int32_t maxMultiSampleCount;
	//D3D_FEATURE_LEVEL featureLevel;

	/* Presentation */
	uint8_t syncInterval;

	//D3D11Backbuffer backbuffer;
	//uint8_t backbufferSizeChanged;
	//Rect prevSrcRect;
	//Rect prevDstRect;

	/* Blend State */
	wrl::ComPtr<ID3D11BlendState> blendState;
	Color blendFactor;
	int32_t multiSampleMask;

	/* Depth Stencil State */
	wrl::ComPtr<ID3D11DepthStencilState> depthStencilState;
	int32_t stencilRef;

	/* Rasterizer State */
	Viewport viewport;
	Rect scissorRect;
	wrl::ComPtr<ID3D11RasterizerState> rasterizerState;

	/* Textures */
	std::vector<D3D11Texture*> vertexTextures;
	std::vector<D3D11Texture*> pixelTextures;
	std::vector<wrl::ComPtr<ID3D11SamplerState>> vertexSamplers;
	std::vector<wrl::ComPtr<ID3D11SamplerState>> pixelSamplers;

	/* Input Assembly */
	wrl::ComPtr<ID3D11InputLayout> inputLayout;
	PrimitiveType topology;
	std::vector<wrl::ComPtr<ID3D11Buffer>> vertexBuffers;
	std::vector<uint32_t> vertexBufferOffsets;
	std::vector<uint32_t> vertexBufferStrides;
	wrl::ComPtr<ID3D11Buffer> indexBuffer;
	size_t indexElementSize;

	///* Resource Caches */
	//PackedStateArray blendStateCache;
	//PackedStateArray depthStencilStateCache;
	//PackedStateArray rasterizerStateCache;
	//PackedStateArray samplerStateCache;
	//PackedVertexBufferBindingsArray inputLayoutCache;

	/* Render Targets */
	int32_t numRenderTargets;
	size_t backBufferWidth;
	size_t backBufferHeight;
	wrl::ComPtr<ID3D11RenderTargetView> swapchainRTView;
	std::vector<wrl::ComPtr<ID3D11RenderTargetView>> renderTargetViews;
	wrl::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	wrl::ComPtr<ID3D11DepthStencilView> depthStencilView;
	DepthFormat currentDepthFormat;


	std::mutex ctxLock;

#ifdef _DEBUG
	DxgiInfoManager infoManager;
#endif
	HRESULT hr;

public:

	virtual ~D3D11Renderer();
	D3D11Renderer(PresentationParameters presentationParameters, uint8_t debugMode);


	// Унаследовано через IRenderer
	virtual void GetDrawableSize(void* window, int32_t* w, int32_t* h) override;
	virtual void SwapBuffers() override;
	virtual void Clear(ClearOptions options, FColor color, float depth, int32_t stencil) override;
	virtual void DrawIndexedPrimitives(PrimitiveType primitiveType, int32_t baseVertex, int32_t minVertexIndex, int32_t numVertices, int32_t startIndex, int32_t primitiveCount, const Buffer* indices, size_t indexElementSize) override;
	virtual void DrawInstancedPrimitives(PrimitiveType primitiveType, int32_t baseVertex, int32_t minVertexIndex, int32_t numVertices, int32_t startIndex, int32_t primitiveCount, int32_t instanceCount, const Buffer* indices, size_t indexElementSize) override;
	virtual void DrawPrimitives(PrimitiveType primitiveType, int32_t vertexStart, int32_t primitiveCount) override;
	virtual void SetViewport(const Viewport& viewport) override;
	virtual void SetScissorRect(Rect scissor) override;
	virtual void GetBlendFactor(Color& blendFactor) override;
	virtual void SetBlendFactor(Color blendFactor) override;
	virtual int32_t GetMultiSampleMask()override;
	virtual void SetMultiSampleMask(int32_t mask)override;
	virtual void SetBlendState(const BlendState& blendState) override;
	virtual void SetDepthStencilState(const DepthStencilState& depthStencilState) override;
	virtual void ApplyRasterizerState(const RasterizerState& rasterizerState) override;
	virtual void VerifyPixelSampler(int32_t index, const Texture* texture, const SamplerState& sampler) override;
	virtual void VerifyVertexSampler(int32_t index,const Texture* texture, const SamplerState& sampler) override;
	virtual void SetRenderTargets(RenderTargetBinding* renderTargets, int32_t numRenderTargets, Renderbuffer* depthStencilBuffer, DepthFormat depthFormat, uint8_t preserveTargetContents) override;
	virtual void ResolveTarget(const RenderTargetBinding& target) override;
	virtual void ResetBackbuffer(const PresentationParameters& presentationParameters) override;
	virtual void ReadBackbuffer(int32_t x, int32_t y, int32_t w, int32_t h, void* data, int32_t dataLength) override;
	virtual void GetBackbufferSize(int32_t* w, int32_t* h) override;
	virtual DepthFormat GetBackbufferDepthFormat() override;
	virtual Texture* CreateTexture2D(int32_t width, int32_t height, int32_t levelCount, uint8_t isRenderTarget) override;
	virtual void AddDisposeTexture(Texture* texture) override;
	virtual void SetTextureData2D(Texture* texture, int32_t x, int32_t y, int32_t w, int32_t h, int32_t level, void* data, int32_t dataLength) override;
	virtual void GetTextureData2D(const Texture* texture, int32_t x, int32_t y, int32_t w, int32_t h, int32_t level, void* data, int32_t dataLength) override;
	virtual Renderbuffer* GenColorRenderbuffer(int32_t width, int32_t height, int32_t multiSampleCount, Texture* texture) override;
	virtual Renderbuffer* GenDepthStencilRenderbuffer(int32_t width, int32_t height, DepthFormat format, int32_t multiSampleCount) override;
	virtual void AddDisposeRenderbuffer(Renderbuffer* renderbuffer) override;
	virtual Buffer* GenVertexBuffer(uint8_t dynamic, BufferUsage usage, int32_t sizeInBytes) override;
	virtual void AddDisposeVertexBuffer(Buffer* buffer) override;
	virtual void SetVertexBufferData(Buffer* buffer, int32_t offsetInBytes, void* data, int32_t elementCount, int32_t elementSizeInBytes, int32_t vertexStride, SetDataOptions options) override;
	virtual void GetVertexBufferData(const Buffer* buffer, int32_t offsetInBytes, void* data, int32_t elementCount, int32_t elementSizeInBytes, int32_t vertexStride) override;
	virtual Buffer* GenIndexBuffer(uint8_t dynamic, BufferUsage usage, int32_t sizeInBytes) override;
	virtual void AddDisposeIndexBuffer(Buffer* buffer) override;
	virtual void SetIndexBufferData(Buffer* buffer, int32_t offsetInBytes, void* data, int32_t dataLength, SetDataOptions options) override;
	virtual void GetIndexBufferData(const Buffer* buffer, int32_t offsetInBytes, void* data, int32_t dataLength) override;



	virtual void ApplyVertexBufferBinding(const VertexBufferBinding* bindings) override;

	/* Private Stuff*/
private:
	void SetPresentationInterval(PresentInterval presentInterval);
	void CreateBackbuffer(const PresentationParameters& parameters);
	void CreateSwapChain(const PresentationParameters& pp);
	void ResizeSwapChain(const PresentationParameters& pp);
	wrl::ComPtr<ID3D11BlendState> FetchBlendState(const BlendState& state);
	wrl::ComPtr<ID3D11DepthStencilState> FetchDepthStencilState(const DepthStencilState& state);
	wrl::ComPtr<ID3D11RasterizerState> FetchRasterizerState(const RasterizerState& state);
	wrl::ComPtr<ID3D11SamplerState> FetchSamplerState(const SamplerState& state);
	void DiscardTargetTextures(ID3D11RenderTargetView** views, int32_t numViews);
	void RestoreTargetTextures();
public:
	/* Shader Stuff */
	//
	//
	//
	//void ApplyPixelShader(const PixelShader* shader);
	//void ApplyVertexShader(const VertexShader* shader);	
	//
	//PixelShader* CreatePixelShader(wrl::ComPtr<ID3D10Blob> shaderData);
	//VertexShader* CreateVertexShader(wrl::ComPtr<ID3D10Blob> shaderData, const D3D11_INPUT_ELEMENT_DESC* inputLayout, UINT inputLayoutSize);

	//void UpdataVertexShader(const VertexShader* shader);






	// Унаследовано через IRenderer
	virtual PixelShader* CompilePixelShader(void* shaderData, size_t dataSize, ShaderDefines defines[ ], size_t definesSize, void* includes, const char* enteryPoint, const char* target, uint16_t flags) override;
	virtual VertexShader* CompileVertexShader(void* shaderData, size_t dataSize, ShaderDefines defines[ ], size_t definesSize, void* includes, const char* enteryPoint, const char* target, uint16_t flags, void* inpitLayout, size_t inputLayoutSize) override;
	
	virtual void AddDisposePixelShader(PixelShader* pixelShader) override;
	virtual void AddDisposeVertexShader(VertexShader* vertexShader) override;


	virtual void ApplyPixelShader(PixelShader* pixelShader) override;
	virtual void ApplyVertexShader(VertexShader* vertexShader) override;

	virtual ConstBuffer* CreateConstBuffer(size_t size) override;
	virtual void VerifyConstBuffers(ConstBuffer** constBuffers, size_t size) override;
	virtual void SetConstBuffer(ConstBuffer* constBuffers, void* data) override;
	virtual void AddDisposeConstBuffer(ConstBuffer* constBuffers) override;


	// Унаследовано через IRenderer
	virtual void ApplyPipelineState(PipelineState* piplineState) override;

};

};