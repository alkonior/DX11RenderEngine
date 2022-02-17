#include "D3D11Renderer.h"

#pragma warning( disable : 26812 )

namespace Renderer {

static inline int32_t PrimitiveVerts(
	PrimitiveType primitiveType,
	int32_t primitiveCount
) {
	switch (primitiveType) {
	case PRIMITIVETYPE_TRIANGLELIST:
		return primitiveCount * 3;
	case PRIMITIVETYPE_TRIANGLESTRIP:
		return primitiveCount + 2;
	case PRIMITIVETYPE_LINELIST:
		return primitiveCount * 2;
	case PRIMITIVETYPE_LINESTRIP:
		return primitiveCount + 1;
	case PRIMITIVETYPE_POINTLIST_EXT:
		return primitiveCount;
	default:

		return 0;
	}
}


D3D11Texture D3D11Texture::NullTexture = D3D11Texture
{
	nullptr,
	nullptr,
	-1,
	0,
	0,
	0,
	NULL
};



void D3D11Renderer::GetDrawableSize(void* window, int32_t* w, int32_t* h) {}

D3D11Renderer::D3D11Renderer(PresentationParameters presentationParameters, uint8_t debugMode) : IRenderer(presentationParameters, debugMode) {

	//DXGI_ADAPTER_DESC adapterDesc;
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	uint32_t flags;

	/* Load CreateDXGIFactory1 */
	//GFX_THROW_INFO(CreateDXGIFactory(__uuidof(IDXGIFactory), &factory));
	//factory->EnumAdapters(0, adapter.GetAddressOf());
	//
	//adapter->GetDesc(&adapterDesc);

	/* Create the D3D11Device */
	flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	if (debugMode) {
		flags |= D3D11_CREATE_DEVICE_DEBUG;
	}


	DXGI_SWAP_CHAIN_DESC swapchainDesc;

	/* Initialize swapchain descriptor */
	swapchainDesc.BufferDesc.Width = 0;
	swapchainDesc.BufferDesc.Height = 0;
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.BufferCount = 3;
	swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//swapchainDesc.BufferDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchainDesc.Flags = 0;
	swapchainDesc.OutputWindow = (HWND)presentationParameters.deviceWindowHandle;

	/* Create the swap chain! */
	//GFX_THROW_INFO(factory->CreateSwapChain(
	//	device.Get(),
	//	&swapchainDesc,
	//	&swapchain
	//));

	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		NULL, /* FIXME: Should be renderer->adapter! */
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&swapchainDesc,
		&swapchain,
		&device,
		&featureLevel,
		&context
	));


	/* Initialize texture and sampler collections */
	vertexTextures.resize(MAX_VERTEXTEXTURE_SAMPLERS, &D3D11Texture::NullTexture);
	vertexSamplers.resize(MAX_VERTEXTEXTURE_SAMPLERS, nullptr);

	pixelTextures.resize(MAX_TEXTURE_SAMPLERS, &D3D11Texture::NullTexture);
	pixelSamplers.resize(MAX_TEXTURE_SAMPLERS, nullptr);

	renderTargetViews.resize(MAX_RENDERTARGET_BINDINGS);

	/* Initialize SetStringMarker support, if available */  //TODO
	//if (featureLevel == D3D_FEATURE_LEVEL_11_1) {
	//	context->QueryInterface(&annotation);
	//
	//	res = ID3D11DeviceContext_QueryInterface(
	//		renderer->context,
	//		&D3D_IID_ID3DUserDefinedAnnotation,
	//		(void**)&renderer->annotation
	//	);
	//	ERROR_CHECK("Could not get UserDefinedAnnotation")
	//} else {
	//	FNA3D_LogInfo("SetStringMarker not supported!");
	//}

	/* Initialize renderer members not covered by SDL_memset('\0') */
	this->debugMode = debugMode;
	blendFactor.r = 0xFF;
	blendFactor.g = 0xFF;
	blendFactor.b = 0xFF;
	blendFactor.a = 0xFF;
	multiSampleMask = -1; /* AKA 0xFFFFFFFF, ugh -flibit */
	topology = (PrimitiveType)-1; /* Force an update */


	/* Set presentation interval */
	SetPresentationInterval(presentationParameters.presentationInterval);


	CreateSwapChain(presentationParameters);
	CreateBackbuffer(presentationParameters);
}

D3D11Renderer::~D3D11Renderer() {
	context->ClearState();
}

void D3D11Renderer::SwapBuffers() {
	//std::lock_guard<std::mutex> guard(ctxLock);
	GFX_THROW_INFO(swapchain->Present(syncInterval, 0));
	//
}

void D3D11Renderer::Clear(ClearOptions options, FColor color, float depth, int32_t stencil) {
	int32_t i;
	uint32_t dsClearFlags;
	float clearColor[4] = { color.x, color.y, color.z, color.w };

	//std::lock_guard<std::mutex> guard(ctxLock);;

	/* Clear color? */
	if (options & ClearOptions::CLEAROPTIONS_TARGET) {
		for (i = 0; i < numRenderTargets; i += 1) {
			/* Clear! */
			GFX_THROW_INFO_ONLY(context->ClearRenderTargetView(renderTargetViews[i].Get(), clearColor));
		}
	}

	/* Clear depth/stencil? */
	dsClearFlags = 0;
	if (options & CLEAROPTIONS_DEPTHBUFFER) {
		dsClearFlags |= D3D11_CLEAR_DEPTH;
	}
	if (options & CLEAROPTIONS_STENCIL) {
		dsClearFlags |= D3D11_CLEAR_STENCIL;
	}
	if (dsClearFlags != 0 && depthStencilView != NULL) {
		/* Clear! */
		GFX_THROW_INFO_ONLY(context->ClearDepthStencilView(
			depthStencilView.Get(),
			dsClearFlags,
			depth,
			(uint8_t)stencil
		));
	}

}

static DXGI_FORMAT IndexType[] =
{
	DXGI_FORMAT_R16_UINT,		/* IndexElementSize.SixteenBits */
	DXGI_FORMAT_R32_UINT		/* IndexElementSize.ThirtyTwoBits */
};

static D3D_PRIMITIVE_TOPOLOGY D3D_Primitive[] =
{
	D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,	/* PrimitiveType.TriangleList */
	D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,	/* PrimitiveType.TriangleStrip */
	D3D_PRIMITIVE_TOPOLOGY_LINELIST,	    /* PrimitiveType.LineList */
	D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,	    /* PrimitiveType.LineStrip */
	D3D_PRIMITIVE_TOPOLOGY_POINTLIST	    /* PrimitiveType.PointListEXT */
};

void D3D11Renderer::DrawIndexedPrimitives(PrimitiveType primitiveType, int32_t baseVertex, int32_t minVertexIndex, int32_t numVertices, int32_t startIndex, int32_t primitiveCount, const Buffer* indices, size_t indexElementSize) {

	D3D11Buffer* d3dIndices = (D3D11Buffer*)indices;

	//std::lock_guard<std::mutex> guard(ctxLock);;

	/* Bind index buffer */
	if (indexBuffer != d3dIndices->handle || indexElementSize != indexElementSize) {
		indexBuffer = d3dIndices->handle;
		indexElementSize = indexElementSize;
		context->IASetIndexBuffer(
			d3dIndices->handle.Get(),
			IndexType[indexElementSize / 16 - 1],
			0
		);
	}

	/* Set up draw state */
	if (topology != primitiveType) {
		topology = primitiveType;
		context->IASetPrimitiveTopology(
			D3D_Primitive[primitiveType]
		);
	}
	/* Draw! */
	GFX_THROW_INFO_ONLY(context->DrawIndexed(
		PrimitiveVerts(primitiveType, primitiveCount),
		(uint32_t)startIndex,
		baseVertex
	));



}

void D3D11Renderer::DrawInstancedPrimitives(PrimitiveType primitiveType, int32_t baseVertex, int32_t minVertexIndex, int32_t numVertices, int32_t startIndex, int32_t primitiveCount, int32_t instanceCount, const Buffer* indices, size_t indexElementSize) {
	D3D11Buffer* d3dIndices = (D3D11Buffer*)indices;
	//std::lock_guard<std::mutex> guard(ctxLock);

	//	//std::lock_guard<std::mutex> guard(ctxLock);;

		/* Bind index buffer */
	if (indexBuffer != d3dIndices->handle || indexElementSize != indexElementSize) {
		indexBuffer = d3dIndices->handle;
		indexElementSize = indexElementSize;
		context->IASetIndexBuffer(
			d3dIndices->handle.Get(),
			IndexType[indexElementSize / 16 - 1],
			0
		);
	}

	/* Set up draw state */
	if (topology != primitiveType) {
		topology = primitiveType;
		context->IASetPrimitiveTopology(
			D3D_Primitive[primitiveType]
		);
	}


	/* Draw! */
	GFX_THROW_INFO_ONLY(context->DrawIndexedInstanced(
		PrimitiveVerts(primitiveType, primitiveCount),
		instanceCount,
		(uint32_t)startIndex,
		baseVertex,
		0
	));



}

void D3D11Renderer::DrawPrimitives(PrimitiveType primitiveType, int32_t vertexStart, int32_t primitiveCount) {

	//std::lock_guard<std::mutex> guard(ctxLock);;

	/* Bind draw state */
	if (topology != primitiveType) {
		topology = primitiveType;
		context->IASetPrimitiveTopology(
			D3D_Primitive[primitiveType]
		);
	}

	/* Draw! */
	GFX_THROW_INFO_ONLY(context->Draw(
		(uint32_t)PrimitiveVerts(primitiveType, primitiveCount),
		(uint32_t)vertexStart
	));


}

void D3D11Renderer::SetViewport(const Viewport& viewport) {
	D3D11_VIEWPORT vp =
	{
		(float)viewport.x,
		(float)viewport.y,
		(float)viewport.w,
		(float)viewport.h,
		viewport.minDepth,
		viewport.maxDepth
	};

	if (this->viewport.x != viewport.x ||
		this->viewport.y != viewport.y ||
		this->viewport.w != viewport.w ||
		this->viewport.h != viewport.h ||
		this->viewport.minDepth != viewport.minDepth ||
		this->viewport.maxDepth != viewport.maxDepth) {
		//std::lock_guard<std::mutex> guard(ctxLock);;
		this->viewport = viewport;
		GFX_THROW_INFO_ONLY(context->RSSetViewports(1, &vp));

	}
}

void D3D11Renderer::SetScissorRect(Rect scissor) {
	D3D11_RECT rect =
	{
		scissor.x,
		scissor.y,
		scissor.x + scissor.w,
		scissor.y + scissor.h
	};

	if (scissorRect.x != scissor.x ||
		scissorRect.y != scissor.y ||
		scissorRect.w != scissor.w ||
		scissorRect.h != scissor.h) {
		//std::lock_guard<std::mutex> guard(ctxLock);;
		scissorRect = scissor;
		GFX_THROW_INFO_ONLY(context->RSSetScissorRects(1, &rect));

	}
}

void D3D11Renderer::GetBlendFactor(Color& blendFactor) {
	blendFactor = this->blendFactor;
}

void D3D11Renderer::SetBlendFactor(Color blendFactor) {
	float factor[4];
	if (this->blendFactor != blendFactor) {
		factor[0] = blendFactor.r / 255.0f;
		factor[1] = blendFactor.g / 255.0f;
		factor[2] = blendFactor.b / 255.0f;
		factor[3] = blendFactor.a / 255.0f;
		this->blendFactor = blendFactor;
		//std::lock_guard<std::mutex> guard(ctxLock);;
		GFX_THROW_INFO_ONLY(context->OMSetBlendState(
			blendState.Get(),
			factor,
			multiSampleMask
		));

	}
}

int32_t D3D11Renderer::GetMultiSampleMask() {
	return multiSampleMask;
}

void D3D11Renderer::SetMultiSampleMask(int32_t mask) {
	float factor[4];
	if (multiSampleMask != mask) {
		multiSampleMask = mask;
		factor[0] = blendFactor.r / 255.0f;
		factor[1] = blendFactor.g / 255.0f;
		factor[2] = blendFactor.b / 255.0f;
		factor[3] = blendFactor.a / 255.0f;
		//std::lock_guard<std::mutex> guard(ctxLock);;
		GFX_THROW_INFO_ONLY(context->OMSetBlendState(
			blendState.Get(),
			factor,
			multiSampleMask
		));

	}
}

void D3D11Renderer::SetBlendState(const BlendState& blendState) {
	float factor[4];
	auto bs = FetchBlendState(blendState);
	//if (renderer->blendState != bs ||
	//	!D3D11_INTERNAL_BlendEquals(&renderer->blendFactor, &blendState->blendFactor) ||
	//	renderer->multiSampleMask != blendState->multiSampleMask) 
	{
		this->blendState = bs;
		factor[0] = blendState.blendFactor.r / 255.0f;
		factor[1] = blendState.blendFactor.g / 255.0f;
		factor[2] = blendState.blendFactor.b / 255.0f;
		factor[3] = blendState.blendFactor.a / 255.0f;
		blendFactor = blendState.blendFactor;
		multiSampleMask = blendState.multiSampleMask;
		//std::lock_guard<std::mutex> guard(ctxLock);;
		GFX_THROW_INFO_ONLY(context->OMSetBlendState(
			bs.Get(),
			factor,
			(UINT)multiSampleMask));

	}
}

void D3D11Renderer::SetDepthStencilState(const DepthStencilState& depthStencilState) {

	auto ds = FetchDepthStencilState(depthStencilState);

	//if (renderer->depthStencilState != ds ||
	//	renderer->stencilRef != depthStencilState->referenceStencil) 
	{
		this->depthStencilState = ds;
		stencilRef = depthStencilState.referenceStencil;
		//std::lock_guard<std::mutex> guard(ctxLock);;
		GFX_THROW_INFO_ONLY(context->OMSetDepthStencilState(
			ds.Get(),
			(UINT)stencilRef
		));

	}
}

void D3D11Renderer::ApplyRasterizerState(const RasterizerState& rasterizerState) {


	auto rs = FetchRasterizerState(rasterizerState);

	//if (this->rasterizerState != rs) 
	{
		this->rasterizerState = rs;
		//std::lock_guard<std::mutex> guard(ctxLock);;
		GFX_THROW_INFO_ONLY(context->RSSetState(
			rs.Get()
		));

	}

}

void D3D11Renderer::VerifyPixelSampler(int32_t index, const Texture* texture, const SamplerState& sampler) {
	if (index < MAX_TEXTURE_SAMPLERS) {
		D3D11Texture* d3dTexture = (D3D11Texture*)texture;
		wrl::ComPtr<ID3D11SamplerState> d3dSamplerState;

		if (d3dTexture->levelCount == -1) {
			if (pixelTextures[index]->levelCount != -1) {
				pixelTextures[index] = &D3D11Texture::NullTexture;
				pixelSamplers[index] = nullptr;
				//std::lock_guard<std::mutex> guard(ctxLock);;
				if (index < MAX_TEXTURE_SAMPLERS) {
					GFX_THROW_INFO_ONLY(context->PSSetShaderResources(
						index,
						1,
						D3D11Texture::NullTexture.shaderView.GetAddressOf()
					));
					GFX_THROW_INFO_ONLY(context->PSSetSamplers(
						index,
						1,
						pixelSamplers[index].GetAddressOf()));
				}

			}
			return;
		}

		/* Bind the correct texture */
		if (d3dTexture->handle != pixelTextures[index]->handle) {
			pixelTextures[index] = d3dTexture;
			//std::lock_guard<std::mutex> guard(ctxLock);;
			GFX_THROW_INFO_ONLY(context->PSSetShaderResources(
				index,
				1,
				d3dTexture->shaderView.GetAddressOf()));

		}

		/* Update the sampler state, if needed */
		d3dSamplerState = FetchSamplerState(sampler);

		if (d3dSamplerState != pixelSamplers[index]) {
			pixelSamplers[index] = d3dSamplerState;
			//std::lock_guard<std::mutex> guard(ctxLock);;
			GFX_THROW_INFO_ONLY(context->PSSetSamplers(
				index,
				1,
				d3dSamplerState.GetAddressOf()));

		}
	}

}

void D3D11Renderer::VerifyVertexSampler(int32_t index, const  Texture* texture, const  SamplerState& sampler) {
	if (index < MAX_VERTEXTEXTURE_SAMPLERS) {
		D3D11Texture* d3dTexture = (D3D11Texture*)texture;
		wrl::ComPtr<ID3D11SamplerState> d3dSamplerState;

		if (d3dTexture->levelCount == -1) {
			if (vertexTextures[index]->levelCount != -1) {
				vertexTextures[index] = &D3D11Texture::NullTexture;
				vertexSamplers[index] = nullptr;
				//std::lock_guard<std::mutex> guard(ctxLock);;
				if (index < MAX_TEXTURE_SAMPLERS) {
					GFX_THROW_INFO_ONLY(context->VSSetShaderResources(
						index,
						1,
						D3D11Texture::NullTexture.shaderView.GetAddressOf()
					));
					GFX_THROW_INFO_ONLY(context->VSSetSamplers(
						index,
						1,
						vertexSamplers[index].GetAddressOf()));
				}

			}
			return;
		}

		/* Bind the correct texture */
		if (d3dTexture->handle != vertexTextures[index]->handle) {
			vertexTextures[index] = d3dTexture;
			//std::lock_guard<std::mutex> guard(ctxLock);;
			GFX_THROW_INFO_ONLY(context->VSSetShaderResources(
				index,
				1,
				d3dTexture->shaderView.GetAddressOf()));

		}

		/* Update the sampler state, if needed */
		d3dSamplerState = FetchSamplerState(sampler);

		//if (d3dSamplerState != renderer->samplers[index]) 
		{
			vertexSamplers[index] = d3dSamplerState;
			//std::lock_guard<std::mutex> guard(ctxLock);;
			GFX_THROW_INFO_ONLY(context->VSSetSamplers(
				index,
				1,
				d3dSamplerState.GetAddressOf()));

		}
	}

}

void D3D11Renderer::SetRenderTargets(RenderTargetBinding* renderTargets, int32_t numRenderTargets, Renderbuffer* depthStencilBuffer, DepthFormat depthFormat, uint8_t preserveTargetContents) {

	D3D11Texture* tex;
	D3D11Renderbuffer* rb;
	ID3D11RenderTargetView* views[MAX_RENDERTARGET_BINDINGS];
	wrl::ComPtr<ID3D11RenderTargetView> comViews[MAX_RENDERTARGET_BINDINGS];
	int32_t i;


	if (numRenderTargets <= 0) {
		views[0] = swapchainRTView.Get();
		comViews[0] = swapchainRTView;

		//currentDepthFormat = backbuffer->depthFormat;
		//depthStencilView = backbuffer->depthStencilView;

		//std::lock_guard<std::mutex> guard(ctxLock);;
		/* No need to discard textures, this is a backbuffer bind */
		context->OMSetRenderTargets(
			1,
			views,
			depthStencilView.Get()
		);
		RestoreTargetTextures();


		renderTargetViews[0] = comViews[0];
		for (i = 1; i < MAX_RENDERTARGET_BINDINGS; i += 1) {
			renderTargetViews[i] = nullptr;
		}
		this->numRenderTargets = 1;
		return;
	}

	/* Update color buffers */
	for (i = 0; i < std::min(numRenderTargets, MAX_RENDERTARGET_BINDINGS); i += 1) {
		if (renderTargets[i].colorBuffer != NULL) {
			rb = (D3D11Renderbuffer*)renderTargets[i].colorBuffer;
			views[i] = rb->color.rtView.Get();
			comViews[i] = rb->color.rtView;
		}
		else {
			tex = (D3D11Texture*)renderTargets[i].texture;

			//if (tex->rtType == FNA3D_RENDERTARGET_TYPE_2D) {
			//	views[i] = tex->twod.rtView;
			//} else if (tex->rtType == FNA3D_RENDERTARGET_TYPE_CUBE) {
			//	views[i] = tex->cube.rtViews[
			//		renderTargets[i].cube.face
			//	];
			//}

			views[i] = tex->rtView.Get();
			comViews[i] = tex->rtView;
		}
	}
	while (i < MAX_RENDERTARGET_BINDINGS) {
		comViews[i] = nullptr;
		views[i++] = nullptr;
	}

	/* Update depth stencil buffer */
	depthStencilView = (depthStencilBuffer == nullptr ?
		nullptr :
		((D3D11Renderbuffer*)depthStencilBuffer)->depth.dsView);
	currentDepthFormat = (depthStencilBuffer == nullptr ?
		DEPTHFORMAT_NONE :
		depthFormat
		);

	/* Actually set the render targets, finally. */
	//std::lock_guard<std::mutex> guard(ctxLock);;
	DiscardTargetTextures(views, numRenderTargets);
	GFX_THROW_INFO_ONLY(context->OMSetRenderTargets(
		numRenderTargets,
		views,
		depthStencilView.Get()
	));
	RestoreTargetTextures();


	/* Remember color attachments */
	for (int i = 0; i < MAX_RENDERTARGET_BINDINGS; i++) {
		renderTargetViews[i] = comViews[i];
	}
	this->numRenderTargets = numRenderTargets;

}

void D3D11Renderer::ResolveTarget(const RenderTargetBinding& target) {
	D3D11Texture* tex = (D3D11Texture*)target.texture;
	D3D11Renderbuffer& rb = (D3D11Renderbuffer&)target.colorBuffer;
	uint32_t slice = 0;

	//std::lock_guard<std::mutex> guard(ctxLock);;

	if (target.multiSampleCount > 0) {
		//if (target->type == FNA3D_RENDERTARGET_TYPE_CUBE) {
		//	slice = target->cube.face;
		//}
		GFX_THROW_INFO_ONLY(context->ResolveSubresource(
			tex->handle.Get(),
			0 + slice * tex->levelCount,
			rb.handle.Get(),
			0,
			DXGI_FORMAT_R8G8B8A8_UNORM
		));
	}

	/* If the target has mipmaps, regenerate them now */
	if (target.levelCount > 1) {
		GFX_THROW_INFO_ONLY(context->GenerateMips(
			tex->shaderView.Get()
		));
	}


}

void D3D11Renderer::ReadBackbuffer(int32_t x, int32_t y, int32_t w, int32_t h, void* data, int32_t dataLength) {
	D3D11Texture* backbufferTexture = new D3D11Texture();

	GFX_THROW_INFO(swapchain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		&backbufferTexture->handle));

	GetTextureData2D(
		backbufferTexture,
		x,
		y,
		w,
		h,
		0,
		data,
		dataLength);

}

void D3D11Renderer::GetBackbufferSize(int32_t* w, int32_t* h) { *w = static_cast<int32_t>(backBufferWidth); *h = static_cast<int32_t>(backBufferHeight); }

DepthFormat D3D11Renderer::GetBackbufferDepthFormat() {
	return currentDepthFormat;
}

Texture* D3D11Renderer::CreateTexture2D(int32_t width, int32_t height, int32_t levelCount, uint8_t isRenderTarget) {
	D3D11Texture* result = new D3D11Texture();
	D3D11_TEXTURE2D_DESC desc;
	D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc;

	/* Initialize descriptor */
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = levelCount;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	if (isRenderTarget) {
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	/* Create the texture */
	GFX_THROW_INFO(device->CreateTexture2D(
		&desc,
		NULL,
		&result->handle
	));

	/* Initialize D3D11Texture */
	result->levelCount = levelCount;
	result->isRenderTarget = isRenderTarget;
	result->width = width;
	result->height = height;

	/* Create the shader resource view */
	GFX_THROW_INFO(device->CreateShaderResourceView(
		result->handle.Get(),
		NULL,
		&result->shaderView
	));

	/* Create the render target view, if applicable */
	if (isRenderTarget) {
		result->isRenderTarget = 1;
		rtViewDesc.Format = desc.Format;
		rtViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtViewDesc.Texture2D.MipSlice = 0;
		GFX_THROW_INFO(device->CreateRenderTargetView(
			result->handle.Get(),
			&rtViewDesc,
			&result->rtView
		));
	}

	return result;
}

void D3D11Renderer::AddDisposeTexture(Texture* texture) {
	D3D11Texture* tex = (D3D11Texture*)texture;

	/* Unbind the texture */
	for (int i = 0; i < MAX_TEXTURE_SAMPLERS; i += 1) {
		if (pixelTextures[i]->handle == tex->handle) {
			pixelTextures[i] = &D3D11Texture::NullTexture;
		}
	}

	/* Unbind and release the render target views, if applicable */
	if (tex->isRenderTarget) {
		for (int i = 0; i < numRenderTargets; i += 1) {
			if (tex->rtView == renderTargetViews[i]) {
				renderTargetViews[i] = nullptr;
			}
		}
		tex->rtView->Release();
	}

	/* Release the shader resource view and texture */
	//tex->shaderView->Release();
	//tex->handle->Release();
	delete tex;
}

void D3D11Renderer::SetTextureData2D(Texture* texture, int32_t x, int32_t y, int32_t w, int32_t h, int32_t level, void* data, int32_t dataLength) {
	D3D11Texture* d3dTexture = (D3D11Texture*)texture;
	D3D11_BOX dstBox;

	/* DXT formats require w and h to be multiples of 4 */
	//if (d3dTexture->format == FNA3D_SURFACEFORMAT_DXT1 ||
	//	d3dTexture->format == FNA3D_SURFACEFORMAT_DXT3 ||
	//	d3dTexture->format == FNA3D_SURFACEFORMAT_DXT5) {
	//	w = (w + 3) & ~3;
	//	h = (h + 3) & ~3;
	//}

	dstBox.left = x;
	dstBox.top = y;
	dstBox.front = 0;
	dstBox.right = x + w;
	dstBox.bottom = y + h;
	dstBox.back = 1;

	//std::lock_guard<std::mutex> guard(ctxLock);;
	GFX_THROW_INFO_ONLY(context->UpdateSubresource(
		d3dTexture->handle.Get(),
		level,
		&dstBox,
		data,
		w * 4,
		0
	));

}
#pragma warning(push)
#pragma warning(disable : 26451)
void D3D11Renderer::GetTextureData2D(const Texture* texture, int32_t x, int32_t y, int32_t w, int32_t h, int32_t level, void* data, int32_t dataLength) {
	D3D11Texture* tex = (D3D11Texture*)texture;
	D3D11_TEXTURE2D_DESC stagingDesc;
	wrl::ComPtr<ID3D11Texture2D> stagingTexture;
	uint32_t subresourceIndex = level;
	unsigned int texW = static_cast<unsigned int>(tex->width >> level);
	unsigned int texH = static_cast<unsigned int>(tex->height >> level);
	D3D11_BOX srcBox = { 0, 0, 0, texW, texH, 1 };
	D3D11_MAPPED_SUBRESOURCE subresource;
	uint8_t* dataPtr = (uint8_t*)data;

	int32_t formatSize = 4;

	/* Create staging texture if needed */
	if (tex->isRenderTarget) {
		stagingTexture = tex->staging;
	}
	else {
		stagingTexture = nullptr;
	}
	if (stagingTexture == nullptr) {
		stagingDesc.Width = static_cast<UINT>(tex->width);
		stagingDesc.Height = static_cast<UINT>(tex->height);
		stagingDesc.MipLevels = tex->levelCount;
		stagingDesc.ArraySize = 1;
		stagingDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		stagingDesc.SampleDesc.Count = 1;
		stagingDesc.SampleDesc.Quality = 0;
		stagingDesc.Usage = D3D11_USAGE_STAGING;
		stagingDesc.BindFlags = 0;
		stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingDesc.MiscFlags = 0;

		GFX_THROW_INFO(device->CreateTexture2D(
			&stagingDesc,
			NULL,
			&stagingTexture
		));

		/* Targets will probably call this a lot, so try to keep this all the time */
		if (tex->isRenderTarget) {
			tex->staging = stagingTexture;
		}
	}


	//std::lock_guard<std::mutex> guard(ctxLock);;

	/* Copy data into staging texture */
	GFX_THROW_INFO_ONLY(context->CopySubresourceRegion(
		stagingTexture.Get(),
		subresourceIndex,
		0,
		0,
		0,
		tex->handle.Get(),
		subresourceIndex,
		&srcBox
	));

	/* Read from the staging texture */
	GFX_THROW_INFO_ONLY(context->Map(
		stagingTexture.Get(),
		subresourceIndex,
		D3D11_MAP_READ,
		0,
		&subresource
	));


	for (int32_t row = y; row < y + h; row += 1) {
		memcpy(
			dataPtr,
			(uint8_t*)subresource.pData + subresource.RowPitch * row + formatSize * x,
			w * formatSize
		);
		dataPtr += formatSize * w;
	}

	GFX_THROW_INFO_ONLY(context->Unmap(
		stagingTexture.Get(),
		subresourceIndex
	));


}

#pragma warning(pop)


Renderbuffer* D3D11Renderer::GenColorRenderbuffer(int32_t width, int32_t height, int32_t multiSampleCount, Texture* texture) {
	D3D11_TEXTURE2D_DESC desc;
	D3D11Renderbuffer* result = new D3D11Renderbuffer(RENDERBUFFER_COLOR);

	/* Initialize the renderbuffer */
	result->multiSampleCount = multiSampleCount;
	result->type = RENDERBUFFER_COLOR;
	//result->color.format = format;

	/* Create the backing texture */
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = (multiSampleCount > 1 ? multiSampleCount : 1);
	desc.SampleDesc.Quality = (
		multiSampleCount > 1 ? D3D11_STANDARD_MULTISAMPLE_PATTERN : 0
		);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	GFX_THROW_INFO(device->CreateTexture2D(
		&desc,
		NULL,
		&result->handle
	));

	/* Create the render target view */
	GFX_THROW_INFO(device->CreateRenderTargetView(
		result->handle.Get(),
		NULL,
		&result->color.rtView
	));

	return result;
}

static DXGI_FORMAT D3D11DepthFormat[] =
{
	DXGI_FORMAT_UNKNOWN,		    /* DepthFormat.None */
	DXGI_FORMAT_D16_UNORM,		    /* DepthFormat.Depth16 */
	DXGI_FORMAT_D24_UNORM_S8_UINT,	/* DepthFormat.Depth24 */
	DXGI_FORMAT_D24_UNORM_S8_UINT	/* DepthFormat.Depth24Stencil8 */
};

Renderbuffer* D3D11Renderer::GenDepthStencilRenderbuffer(int32_t width, int32_t height, DepthFormat format, int32_t multiSampleCount) {
	D3D11_TEXTURE2D_DESC desc;
	D3D11Renderbuffer* result = new D3D11Renderbuffer(RENDERBUFFER_DEPTH);

	/* Initialize the renderbuffer */
	result->multiSampleCount = multiSampleCount;
	result->type = RENDERBUFFER_DEPTH;
	result->depth.format = format;

	/* Create the backing texture */
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = D3D11DepthFormat[format];
	desc.SampleDesc.Count = (multiSampleCount > 1 ? multiSampleCount : 1);
	desc.SampleDesc.Quality = (
		multiSampleCount > 1 ? D3D11_STANDARD_MULTISAMPLE_PATTERN : 0
		);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	GFX_THROW_INFO(device->CreateTexture2D(
		&desc,
		NULL,
		&result->handle
	));

	/* Create the render target view */
	GFX_THROW_INFO(device->CreateDepthStencilView(
		result->handle.Get(),
		NULL,
		&result->depth.dsView
	));

	return result;
}

void D3D11Renderer::AddDisposeRenderbuffer(Renderbuffer* renderbuffer) {
	D3D11Renderbuffer* d3dRenderbuffer = (D3D11Renderbuffer*)renderbuffer;
	int32_t i;

	if (d3dRenderbuffer->type == RENDERBUFFER_DEPTH) {
		if (d3dRenderbuffer->depth.dsView == depthStencilView) {
			depthStencilView = nullptr;
		}
		d3dRenderbuffer->depth.dsView = nullptr;
	}
	else {
		for (i = 0; i < MAX_RENDERTARGET_BINDINGS; i += 1) {
			if (d3dRenderbuffer->color.rtView == renderTargetViews[i]) {
				renderTargetViews[i] = nullptr;
			}
		}
		d3dRenderbuffer->color.rtView = nullptr;
	}

	d3dRenderbuffer->handle = nullptr;
	delete d3dRenderbuffer;
}

Buffer* D3D11Renderer::GenVertexBuffer(uint8_t dynamic, BufferUsage usage, int32_t sizeInBytes) {
	D3D11Buffer* result = new D3D11Buffer();
	D3D11_BUFFER_DESC desc;

	/* Initialize the descriptor */
	desc.ByteWidth = sizeInBytes;
	desc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	/* Make the buffer */
	GFX_THROW_INFO(device->CreateBuffer(
		&desc,
		NULL,
		&result->handle
	));

	/* Return the result */
	result->dynamic = dynamic;
	result->size = desc.ByteWidth;
	return result;
}

void D3D11Renderer::AddDisposeVertexBuffer(Buffer* buffer) {
	D3D11Buffer* d3dBuffer = (D3D11Buffer*)buffer;
	ID3D11Buffer* nullVertexBuffers[] = { NULL };
	uint32_t whatever[1] = { 0 };
	//int32_t i;

	//for (i = 0; i < MAX_BOUND_VERTEX_BUFFERS; i += 1) {
	//	if (renderer->vertexBuffers[i] == d3dBuffer->handle) {
	//		renderer->vertexBuffers[i] = NULL;
	//		SDL_LockMutex(renderer->ctxLock);
	//		ID3D11DeviceContext_IASetVertexBuffers(
	//			renderer->context,
	//			i,
	//			1,
	//			nullVertexBuffers,
	//			whatever,
	//			whatever
	//		);
	//		SDL_UnlockMutex(renderer->ctxLock);
	//	}
	//}

	d3dBuffer->handle = nullptr;
	delete d3dBuffer;
}

void D3D11Renderer::SetVertexBufferData(Buffer* buffer, int32_t offsetInBytes, void* data, int32_t elementCount, int32_t elementSizeInBytes, int32_t vertexStride, SetDataOptions options) {
	D3D11Buffer* d3dBuffer = (D3D11Buffer*)buffer;
	D3D11_MAPPED_SUBRESOURCE subres = { 0, 0, 0 };
	int32_t dataLen = vertexStride * elementCount;
	D3D11_BOX dstBox = { (UINT)offsetInBytes, 0, 0, (UINT)(offsetInBytes + dataLen), 1, 1 };

	//std::lock_guard<std::mutex> guard(ctxLock);;
	if (d3dBuffer->dynamic) {
		if (debugMode &&
			options == SETDATAOPTIONS_NONE &&
			dataLen < d3dBuffer->size) {
			throw InfoException(__LINE__, __FILE__, { "Dynamic buffer using SetDataOptions.None, expect bad performance and broken output!" });
		}

		GFX_THROW_INFO_ONLY(context->Map(
			d3dBuffer->handle.Get(),
			0,
			options == SETDATAOPTIONS_NOOVERWRITE ?
			D3D11_MAP_WRITE_NO_OVERWRITE :
			D3D11_MAP_WRITE_DISCARD,
			0,
			&subres
		));
		memcpy(
			(uint8_t*)subres.pData + offsetInBytes,
			data,
			dataLen
		);
		GFX_THROW_INFO_ONLY(context->Unmap(d3dBuffer->handle.Get(), 0));
	}
	else {
		GFX_THROW_INFO_ONLY(context->UpdateSubresource(
			d3dBuffer->handle.Get(),
			0,
			&dstBox,
			data,
			dataLen,
			dataLen
		));
	}

}

void D3D11Renderer::GetVertexBufferData(const Buffer* buffer, int32_t offsetInBytes, void* data, int32_t elementCount, int32_t elementSizeInBytes, int32_t vertexStride) {
	const D3D11Buffer* d3dBuffer = (const D3D11Buffer*)buffer;
	D3D11_BUFFER_DESC desc;
	wrl::ComPtr<ID3D11Buffer> stagingBuffer;
	int32_t dataLength = vertexStride * elementCount;
	uint8_t* src, * dst;
	int32_t i;
	D3D11_MAPPED_SUBRESOURCE subres;
	D3D11_BOX srcBox = { (UINT)offsetInBytes, 0, 0, (UINT)offsetInBytes + (UINT)dataLength, 1, 1 };

	/* Create staging buffer */
	desc.ByteWidth = d3dBuffer->size;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	device->CreateBuffer(
		&desc,
		NULL,
		&stagingBuffer
	);

	//std::lock_guard<std::mutex> guard(ctxLock);;

	/* Copy data into staging buffer */
	GFX_THROW_INFO_ONLY(context->CopySubresourceRegion(
		stagingBuffer.Get(),
		0,
		0,
		0,
		0,
		d3dBuffer->handle.Get(),
		0,
		&srcBox
	));

	/* Read from the staging buffer */
	GFX_THROW_INFO_ONLY(context->Map(
		stagingBuffer.Get(),
		0,
		D3D11_MAP_READ,
		0,
		&subres
	));
	if (elementSizeInBytes < vertexStride) {
		dst = (uint8_t*)data;
		src = (uint8_t*)subres.pData;
		for (i = 0; i < elementCount; i += 1) {
			memcpy(dst, src, elementSizeInBytes);
			dst += elementSizeInBytes;
			src += vertexStride;
		}
	}
	else {
		memcpy(
			data,
			subres.pData,
			dataLength
		);
	}
	GFX_THROW_INFO_ONLY(context->Unmap(
		stagingBuffer.Get(),
		0
	));



}

Buffer* D3D11Renderer::GenIndexBuffer(uint8_t dynamic, BufferUsage usage, int32_t sizeInBytes) {
	D3D11Buffer* result = new D3D11Buffer();
	D3D11_BUFFER_DESC desc;

	/* Initialize the descriptor */
	desc.ByteWidth = sizeInBytes;
	desc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	/* Make the buffer */
	device->CreateBuffer(
		&desc,
		NULL,
		&result->handle
	);

	/* Return the result */
	result->dynamic = dynamic;
	result->size = desc.ByteWidth;
	return result;
}

void D3D11Renderer::AddDisposeIndexBuffer(Buffer* buffer) {
	D3D11Buffer* d3dBuffer = (D3D11Buffer*)buffer;

	if (d3dBuffer->handle == indexBuffer) {
		indexBuffer = nullptr;
		//std::lock_guard<std::mutex> guard(ctxLock);;
		context->IASetIndexBuffer(
			NULL,
			DXGI_FORMAT_R16_UINT,
			0
		);

	}

	d3dBuffer->handle = nullptr;
	delete d3dBuffer;
}

void D3D11Renderer::SetIndexBufferData(Buffer* buffer, int32_t offsetInBytes, void* data, int32_t dataLength, SetDataOptions options) {

	D3D11Buffer* d3dBuffer = (D3D11Buffer*)buffer;
	D3D11_MAPPED_SUBRESOURCE subres = { 0, 0, 0 };
	D3D11_BOX dstBox = { (UINT)offsetInBytes, 0, 0, (UINT)offsetInBytes + (UINT)dataLength, 1, 1 };


	//std::lock_guard<std::mutex> guard(ctxLock);;
	if (d3dBuffer->dynamic) {
		if (debugMode &&
			options == SETDATAOPTIONS_NONE &&
			dataLength < d3dBuffer->size) {
			throw InfoException(__LINE__, __FILE__, { "Dynamic buffer using SetDataOptions.None, expect bad performance and broken output!" });
		}

		GFX_THROW_INFO_ONLY(context->Map(
			d3dBuffer->handle.Get(),
			0,
			options == SETDATAOPTIONS_NOOVERWRITE ?
			D3D11_MAP_WRITE_NO_OVERWRITE :
			D3D11_MAP_WRITE_DISCARD,
			0,
			&subres
		));
		memcpy(
			(uint8_t*)subres.pData + offsetInBytes,
			data,
			dataLength
		);
		GFX_THROW_INFO_ONLY(context->Unmap(
			d3dBuffer->handle.Get(),
			0
		));
	}
	else {
		GFX_THROW_INFO_ONLY(context->UpdateSubresource(
			d3dBuffer->handle.Get(),
			0,
			&dstBox,
			data,
			dataLength,
			dataLength
		));
	}

}

void D3D11Renderer::GetIndexBufferData(const Buffer* buffer, int32_t offsetInBytes, void* data, int32_t dataLength) {
	D3D11Buffer* d3dBuffer = (D3D11Buffer*)buffer;
	D3D11_BUFFER_DESC desc;
	wrl::ComPtr<ID3D11Buffer> stagingBuffer;
	D3D11_MAPPED_SUBRESOURCE subres;
	D3D11_BOX srcBox = { (UINT)offsetInBytes, 0, 0, (UINT)offsetInBytes + (UINT)dataLength, 1, 1 };

	/* Create staging buffer */
	desc.ByteWidth = d3dBuffer->size;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	GFX_THROW_INFO(device->CreateBuffer(
		&desc,
		NULL,
		&stagingBuffer
	));
	//std::lock_guard<std::mutex> guard(ctxLock);;

	/* Copy data into staging buffer */
	GFX_THROW_INFO_ONLY(context->CopySubresourceRegion(
		stagingBuffer.Get(),
		0,
		0,
		0,
		0,
		d3dBuffer->handle.Get(),
		0,
		&srcBox
	));

	/* Read from the staging buffer */
	GFX_THROW_INFO_ONLY(context->Map(
		stagingBuffer.Get(),
		0,
		D3D11_MAP_READ,
		0,
		&subres
	));
	memcpy(
		data,
		subres.pData,
		dataLength
	);
	GFX_THROW_INFO_ONLY(context->Unmap(
		stagingBuffer.Get(),
		0
	));


}

void D3D11Renderer::SetPresentationInterval(PresentInterval presentInterval) {

	switch (presentInterval) {
	case PRESENTINTERVAL_DEFAULT:
	case PRESENTINTERVAL_ONE:
	{
		syncInterval = 1;
		break;
	}
	case PRESENTINTERVAL_TWO:
	{
		syncInterval = 2;
		break;
	}
	case PRESENTINTERVAL_IMMEDIATE:
	{
		syncInterval = 0;
		break;
	}
	}

}




void D3D11Renderer::CreateBackbuffer(const PresentationParameters& parameters) {
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RENDER_TARGET_VIEW_DESC swapchainViewDesc;
	wrl::ComPtr<ID3D11Texture2D> swapchainTexture;



	backBufferWidth = parameters.backBufferWidth;
	backBufferHeight = parameters.backBufferHeight;

	//backBufferWidth = 1024;w:1024,h:576
	//backBufferHeight = 583;

	if (parameters.depthStencilFormat != DEPTHFORMAT_NONE) {
		depthStencilDesc.Width = parameters.backBufferWidth;
		depthStencilDesc.Height = parameters.backBufferHeight;

		depthStencilDesc.Width = 1024;
		depthStencilDesc.Height = 576;

		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = D3D11DepthFormat[parameters.depthStencilFormat];
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;
		GFX_THROW_INFO(device->CreateTexture2D(
			&depthStencilDesc,
			NULL,
			&depthStencilBuffer
		));



		/* Update the depth-stencil view */
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.Flags = 0;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		GFX_THROW_INFO(device->CreateDepthStencilView(
			depthStencilBuffer.Get(),
			&depthStencilViewDesc,
			&depthStencilView
		));
	}


	/* Create a render target view for the swapchain */
	swapchainViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	swapchainViewDesc.Texture2D.MipSlice = 0;

	GFX_THROW_INFO(swapchain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		&swapchainTexture
	));

	GFX_THROW_INFO(device->CreateRenderTargetView(
		swapchainTexture.Get(),
		NULL,
		&swapchainRTView
	));

	/* This is the default render target */
	SetRenderTargets(
		NULL,
		0,
		NULL,
		DEPTHFORMAT_NONE,
		0
	);

}


void D3D11Renderer::ResetBackbuffer(const PresentationParameters& presentationParameters) {

	CreateBackbuffer(
		presentationParameters
	);
	SetPresentationInterval(
		presentationParameters.presentationInterval
	);

}

void D3D11Renderer::CreateSwapChain(const PresentationParameters& pp) {

}
void D3D11Renderer::ResizeSwapChain(const PresentationParameters& pp) {
	int w, h;
	GetDrawableSize((HWND)pp.deviceWindowHandle, &w, &h);
	GFX_THROW_INFO(swapchain->ResizeBuffers(
		0,			/* keep # of buffers the same */
		w,
		h,
		DXGI_FORMAT_UNKNOWN,	/* keep the old format */
		0));
}

static D3D11_BLEND BlendMode[] =
{
	D3D11_BLEND_ONE,		        /* Blend.One */
	D3D11_BLEND_ZERO,		        /* Blend.Zero */
	D3D11_BLEND_SRC_COLOR,		    /* Blend.SourceColor */
	D3D11_BLEND_INV_SRC_COLOR,	    /* Blend.InverseSourceColor */
	D3D11_BLEND_SRC_ALPHA,		    /* Blend.SourceAlpha */
	D3D11_BLEND_INV_SRC_ALPHA,	    /* Blend.InverseSourceAlpha */
	D3D11_BLEND_DEST_COLOR,		    /* Blend.DestinationColor */
	D3D11_BLEND_INV_DEST_COLOR,	    /* Blend.InverseDestinationColor */
	D3D11_BLEND_DEST_ALPHA,		    /* Blend.DestinationAlpha */
	D3D11_BLEND_INV_DEST_ALPHA,	    /* Blend.InverseDestinationAlpha */
	D3D11_BLEND_BLEND_FACTOR,	    /* Blend.BlendFactor */
	D3D11_BLEND_INV_BLEND_FACTOR,	/* Blend.InverseBlendFactor */
	D3D11_BLEND_SRC_ALPHA_SAT	    /* Blend.SourceAlphaSaturation */
};

static D3D11_BLEND BlendModeAlpha[] =
{
	D3D11_BLEND_ONE,		         /* Blend.One */
	D3D11_BLEND_ZERO,		         /* Blend.Zero */
	D3D11_BLEND_SRC_ALPHA,		     /* Blend.SourceColor */
	D3D11_BLEND_INV_SRC_ALPHA,	     /* Blend.InverseSourceColor */
	D3D11_BLEND_SRC_ALPHA,		     /* Blend.SourceAlpha */
	D3D11_BLEND_INV_SRC_ALPHA,	     /* Blend.InverseSourceAlpha */
	D3D11_BLEND_DEST_ALPHA,		     /* Blend.DestinationColor */
	D3D11_BLEND_INV_DEST_ALPHA,	     /* Blend.InverseDestinationColor */
	D3D11_BLEND_DEST_ALPHA,		     /* Blend.DestinationAlpha */
	D3D11_BLEND_INV_DEST_ALPHA,	     /* Blend.InverseDestinationAlpha */
	D3D11_BLEND_BLEND_FACTOR,	     /* Blend.BlendFactor */
	D3D11_BLEND_INV_BLEND_FACTOR,	 /* Blend.InverseBlendFactor */
	D3D11_BLEND_SRC_ALPHA_SAT	     /* Blend.SourceAlphaSaturation */
};

static D3D11_BLEND_OP BlendOperation[] =
{
	D3D11_BLEND_OP_ADD,		        /* BlendFunction.Add */
	D3D11_BLEND_OP_SUBTRACT,	    /* BlendFunction.Subtract */
	D3D11_BLEND_OP_REV_SUBTRACT,	/* BlendFunction.ReverseSubtract */
	D3D11_BLEND_OP_MAX,		        /* BlendFunction.Max */
	D3D11_BLEND_OP_MIN		        /* BlendFunction.Min */
};

static D3D11_COMPARISON_FUNC CompareFunc[] =
{
	D3D11_COMPARISON_ALWAYS,	    /* CompareFunction.Always */
	D3D11_COMPARISON_NEVER,		    /* CompareFunction.Never */
	D3D11_COMPARISON_LESS,		    /* CompareFunction.Less */
	D3D11_COMPARISON_LESS_EQUAL,    /* CompareFunction.LessEqual */
	D3D11_COMPARISON_EQUAL,		    /* CompareFunction.Equal */
	D3D11_COMPARISON_GREATER_EQUAL,	/* CompareFunction.GreaterEqual */
	D3D11_COMPARISON_GREATER,	    /* CompareFunction.Greater */
	D3D11_COMPARISON_NOT_EQUAL	    /* CompareFunction.NotEqual */
};



wrl::ComPtr<ID3D11BlendState> D3D11Renderer::FetchBlendState(const BlendState& state) {
	D3D11_BLEND_DESC desc;
	memset(&desc, 0, sizeof(desc));

	if (hashBS.count(&state)) {
		return hashBS.at(&state);
	}

	wrl::ComPtr<ID3D11BlendState> result;



	/* We need to make a new blend state... */
	desc.AlphaToCoverageEnable = 0;
	desc.IndependentBlendEnable = 0;
	desc.RenderTarget[0].BlendEnable = state.enabled &&
		!(
			state.colorSourceBlend == BLEND_ONE &&
			state.colorDestinationBlend == BLEND_ZERO &&
			state.alphaSourceBlend == BLEND_ONE &&
			state.alphaDestinationBlend == BLEND_ZERO
			);
	if (desc.RenderTarget[0].BlendEnable) {
		desc.RenderTarget[0].BlendOp = BlendOperation[state.colorBlendFunction];
		desc.RenderTarget[0].BlendOpAlpha = BlendOperation[state.alphaBlendFunction];
		desc.RenderTarget[0].DestBlend = BlendMode[state.colorDestinationBlend];
		desc.RenderTarget[0].DestBlendAlpha = BlendModeAlpha[state.alphaDestinationBlend];
		desc.RenderTarget[0].SrcBlend = BlendMode[state.colorSourceBlend];
		desc.RenderTarget[0].SrcBlendAlpha = BlendModeAlpha[state.alphaSourceBlend];
	}

	/* All other states should match for all targets... */
	desc.RenderTarget[1] = desc.RenderTarget[0];
	desc.RenderTarget[2] = desc.RenderTarget[0];
	desc.RenderTarget[3] = desc.RenderTarget[0];

	/* ... except RenderTargetWriteMask. */
	desc.RenderTarget[0].RenderTargetWriteMask = ((UINT)state.colorWriteEnable);
	desc.RenderTarget[1].RenderTargetWriteMask = ((UINT)state.colorWriteEnable1);
	desc.RenderTarget[2].RenderTargetWriteMask = ((UINT)state.colorWriteEnable2);
	desc.RenderTarget[3].RenderTargetWriteMask = ((UINT)state.colorWriteEnable3);

	/* Bake the state! */
	GFX_THROW_INFO(device->CreateBlendState(&desc, &result));

	hashBS[&state] = result;
	/* Return the state! */
	return result;
}

static D3D11_STENCIL_OP StencilOp[] =
{
	D3D11_STENCIL_OP_KEEP,		/* StencilOperation.Keep */
	D3D11_STENCIL_OP_ZERO,		/* StencilOperation.Zero */
	D3D11_STENCIL_OP_REPLACE,	/* StencilOperation.Replace */
	D3D11_STENCIL_OP_INCR,		/* StencilOperation.Increment */
	D3D11_STENCIL_OP_DECR,		/* StencilOperation.Decrement */
	D3D11_STENCIL_OP_INCR_SAT,	/* StencilOperation.IncrementSaturation */
	D3D11_STENCIL_OP_DECR_SAT,	/* StencilOperation.DecrementSaturation */
	D3D11_STENCIL_OP_INVERT		/* StencilOperation.Invert */
};

wrl::ComPtr<ID3D11DepthStencilState> D3D11Renderer::FetchDepthStencilState(const DepthStencilState& state) {
	D3D11_DEPTH_STENCIL_DESC desc;
	D3D11_DEPTH_STENCILOP_DESC front, back;

	if (hashDSS.count(&state)) {
		return hashDSS.at(&state);
	}

	wrl::ComPtr<ID3D11DepthStencilState> result;

	/* We have to make a new depth stencil state... */
	desc.DepthEnable = state.depthBufferEnable;
	desc.DepthWriteMask = (state.depthBufferEnable && state.depthBufferWriteEnable ?
		D3D11_DEPTH_WRITE_MASK_ALL :
		D3D11_DEPTH_WRITE_MASK_ZERO);

	desc.DepthFunc = CompareFunc[
		state.depthBufferFunction
	];
	desc.StencilEnable = state.stencilEnable;
	desc.StencilReadMask = (UINT8)state.stencilMask;
	desc.StencilWriteMask = (UINT8)state.stencilWriteMask;
	front.StencilDepthFailOp = StencilOp[state.stencilDepthBufferFail];
	front.StencilFailOp = StencilOp[state.stencilFail];
	front.StencilFunc = CompareFunc[state.stencilFunction];
	front.StencilPassOp = StencilOp[state.stencilPass];

	if (state.twoSidedStencilMode) {
		back.StencilDepthFailOp = StencilOp[state.ccwStencilDepthBufferFail];
		back.StencilFailOp = StencilOp[state.ccwStencilFail];
		back.StencilFunc = CompareFunc[state.ccwStencilFunction];
		back.StencilPassOp = StencilOp[state.ccwStencilPass];
	}
	else {
		back = front;
	}
	desc.FrontFace = front;
	desc.BackFace = back;

	/* Bake the state! */
	GFX_THROW_INFO(device->CreateDepthStencilState(
		&desc,
		&result
	));

	hashDSS[&state] = result;

	/* Return the state! */
	return result;
}

float DepthBiasScale[] =
{
	0.0f,				    /* DepthFormat.None */
	(float)((1 << 16) - 1),	/* DepthFormat.Depth16 */
	(float)((1 << 24) - 1),	/* DepthFormat.Depth24 */
	(float)((1 << 24) - 1) 	/* DepthFormat.Depth24Stencil8 */
};

static D3D11_CULL_MODE D3D11CullMode[] =
{
	D3D11_CULL_NONE,		/* CullMode.None */
	D3D11_CULL_BACK,		/* CullMode.CullClockwiseFace */
	D3D11_CULL_FRONT 		/* CullMode.CullCounterClockwiseFace */
};

static D3D11_FILL_MODE D3D11FillMode[] =
{
	D3D11_FILL_SOLID,		/* FillMode.Solid */
	D3D11_FILL_WIREFRAME		/* FillMode.WireFrame */
};

wrl::ComPtr<ID3D11RasterizerState> D3D11Renderer::FetchRasterizerState(const RasterizerState& state) {
	float depthBias;
	D3D11_RASTERIZER_DESC desc;


	if (hashRS.count(&state)) {
		return hashRS.at(&state);
	}

	wrl::ComPtr<ID3D11RasterizerState> result;



	depthBias = state.depthBias * DepthBiasScale[currentDepthFormat];



	/* We have to make a new rasterizer state... */
	desc.AntialiasedLineEnable = 0;
	desc.CullMode = D3D11CullMode[state.cullMode];
	desc.DepthBias = (int32_t)depthBias;
	desc.DepthBiasClamp = D3D11_FLOAT32_MAX;
	desc.DepthClipEnable = 1;
	desc.FillMode = D3D11FillMode[state.fillMode];
	desc.FrontCounterClockwise = 1;
	desc.MultisampleEnable = state.multiSampleAntiAlias;
	desc.ScissorEnable = state.scissorTestEnable;
	desc.SlopeScaledDepthBias = state.slopeScaleDepthBias;

	/* Bake the state! */
	GFX_THROW_INFO(device->CreateRasterizerState(
		&desc,
		&result
	));

	hashRS[(&state)] = result;
	/* Return the state! */
	return result;
}


static D3D11_TEXTURE_ADDRESS_MODE Wrap[] =
{
	D3D11_TEXTURE_ADDRESS_WRAP,	    /* TextureAddressMode.Wrap */
	D3D11_TEXTURE_ADDRESS_CLAMP,	/* `AddressMode.Clamp */
	D3D11_TEXTURE_ADDRESS_MIRROR	/* TextureAddressMode.Mirror */
};

static D3D11_FILTER Filter[] =
{
	D3D11_FILTER_MIN_MAG_MIP_LINEAR,	     	     /* TextureFilter.Linear */
	D3D11_FILTER_MIN_MAG_MIP_POINT,		     	     /* TextureFilter.Point */
	D3D11_FILTER_ANISOTROPIC,			             /* TextureFilter.Anisotropic */
	D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,		     /* TextureFilter.LinearMipPoint */
	D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,		     /* TextureFilter.PointMipLinear */
	D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,	 /* TextureFilter.MinLinearMagPointMipLinear */
	D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,		     /* TextureFilter.MinLinearMagPointMipPoint */
	D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,		     /* TextureFilter.MinPointMagLinearMipLinear */
	D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT	     /* TextureFilter.MinPointMagLinearMipPoint */
};

wrl::ComPtr<ID3D11SamplerState> D3D11Renderer::FetchSamplerState(const SamplerState& state) {
	wrl::ComPtr<ID3D11SamplerState> result;
	D3D11_SAMPLER_DESC desc;

	if (hashSS.count(&state)) {
		return hashSS.at(&state);
	}


	/* We have to make a new sampler state... */
	desc.AddressU = Wrap[state.addressU];
	desc.AddressV = Wrap[state.addressV];
	desc.AddressW = Wrap[state.addressW];
	desc.BorderColor[0] = 1.0f;
	desc.BorderColor[1] = 1.0f;
	desc.BorderColor[2] = 1.0f;
	desc.BorderColor[3] = 1.0f;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.Filter = Filter[state.filter];
	desc.MaxAnisotropy = (uint32_t)state.maxAnisotropy;
	desc.MaxLOD = (float)state.maxMipLevel;
	desc.MinLOD = (float)state.minMipLevel;
	desc.MipLODBias = state.mipMapLevelOfDetailBias;

	/* Bake the state! */
	GFX_THROW_INFO(device->CreateSamplerState(
		&desc,
		&result
	));


	hashSS[(&state)] = result;

	/* Return the state! */
	return result;
}

void D3D11Renderer::DiscardTargetTextures(ID3D11RenderTargetView** views, int32_t numViews) {
	/* For textures that are still bound while this target is about to
		 * become active, rebind. D3D11 implicitly unsets these to prevent
		 * simultaneous read/write, but we still have to be explicit to avoid
		 * warnings from the debug layer.
		 * -flibit
		 */
	uint8_t bound;
	for (int32_t i = 0; i < numViews; i += 1) {
		auto view = views[i];
		for (int32_t j = 0; j < MAX_TEXTURE_SAMPLERS; j += 1) {
			const D3D11Texture* texture = pixelTextures[j];
			if (!texture->isRenderTarget) {
				continue;
			}
			//if (texture->rtType == FNA3D_RENDERTARGET_TYPE_2D) {
			//	bound = (texture->twod.rtView == view);
			//} else {
			//	bound = 0;
			//	for (k = 0; k < 6; k += 1) {
			//		if (texture->cube.rtViews[k] == view) {
			//			bound = 1;
			//			break;
			//		}
			//	}
			//}
			bound = (texture->rtView.Get() == view);
			if (bound) {
				context->PSSetShaderResources(
					j,
					1,
					D3D11Texture::NullTexture.shaderView.GetAddressOf()
				);
				context->PSSetSamplers(
					j,
					1,
					pixelSamplers[j].GetAddressOf()
				);
			}
		}

		for (int32_t j = 0; j < MAX_VERTEXTEXTURE_SAMPLERS; j += 1) {
			const D3D11Texture* texture = vertexTextures[j];
			if (!texture->isRenderTarget) {
				continue;
			}
			//if (texture->rtType == FNA3D_RENDERTARGET_TYPE_2D) {
			//	bound = (texture->twod.rtView == view);
			//} else {
			//	bound = 0;
			//	for (k = 0; k < 6; k += 1) {
			//		if (texture->cube.rtViews[k] == view) {
			//			bound = 1;
			//			break;
			//		}
			//	}
			//}
			bound = (texture->rtView.Get() == view);
			if (bound) {
				context->VSSetShaderResources(
					j,
					1,
					D3D11Texture::NullTexture.shaderView.GetAddressOf()
				);
				context->VSSetSamplers(
					j,
					1,
					vertexSamplers[j].GetAddressOf()
				);
			}
		}
	}
}

void D3D11Renderer::RestoreTargetTextures() {
	/*
	 * For textures that were bound while this target was active, rebind.
	 * D3D11 implicitly unsets these to prevent simultaneous read/write.
	 * -flibit
	 */
	uint8_t bound;
	for (int32_t i = 0; i < numRenderTargets; i += 1) {
		auto view = renderTargetViews[i];
		for (int32_t j = 0; j < MAX_TEXTURE_SAMPLERS; j += 1) {
			auto texture = pixelTextures[j];
			if (!texture->isRenderTarget) {
				continue;
			}

			//if (texture->rtType == FNA3D_RENDERTARGET_TYPE_2D) {
			//	bound = (texture->twod.rtView == view);
			//} else {
			//	bound = 0;
			//	for (k = 0; k < 6; k += 1) {
			//		if (texture->cube.rtViews[k] == view) {
			//			bound = 1;
			//			break;
			//		}
			//	}
			//}

			bound = (texture->rtView == view);
			if (bound) {
				if (j < MAX_TEXTURE_SAMPLERS) {
					context->PSSetShaderResources(
						j,
						1,
						texture->shaderView.GetAddressOf()
					);
					context->PSSetSamplers(
						j,
						1,
						pixelSamplers[j].GetAddressOf()
					);
				}
			}
		}
		for (int32_t j = 0; j < MAX_VERTEXTEXTURE_SAMPLERS; j += 1) {
			auto texture = vertexTextures[j];
			if (!texture->isRenderTarget) {
				continue;
			}

			//if (texture->rtType == FNA3D_RENDERTARGET_TYPE_2D) {
			//	bound = (texture->twod.rtView == view);
			//} else {
			//	bound = 0;
			//	for (k = 0; k < 6; k += 1) {
			//		if (texture->cube.rtViews[k] == view) {
			//			bound = 1;
			//			break;
			//		}
			//	}
			//}

			bound = (texture->rtView == view);
			if (bound) {
				if (j < MAX_TEXTURE_SAMPLERS) {
					context->VSSetShaderResources(
						j,
						1,
						texture->shaderView.GetAddressOf()
					);
					context->VSSetSamplers(
						j,
						1,
						vertexSamplers[j].GetAddressOf()
					);
				}
			}
		}
	}
}



void D3D11Renderer::ApplyVertexBufferBinding(const VertexBufferBinding* vertexBuffer) {
	if (vertexBuffer->buffersCount == 1) {
		//std::lock_guard<std::mutex> guard(ctxLock);;
		GFX_THROW_INFO_ONLY(context->IASetVertexBuffers(0u, 1u, ((D3D11Buffer*)(*vertexBuffer->vertexBuffers))->handle.GetAddressOf(), vertexBuffer->vertexStride, vertexBuffer->vertexOffset));

	}
	else {
		static std::vector<ID3D11Buffer*> buffers(16);
		for (size_t i = 0; i < vertexBuffer->buffersCount; i++) {
			buffers[i] = ((D3D11Buffer*)(vertexBuffer->vertexBuffers)[i])->handle.Get();
		}
		//std::lock_guard<std::mutex> guard(ctxLock);;
		GFX_THROW_INFO_ONLY(context->IASetVertexBuffers(0u, (UINT)vertexBuffer->buffersCount, buffers.data(), vertexBuffer->vertexStride, vertexBuffer->vertexOffset));

	}
}


//PixelShader* D3D11Renderer::CreatePixelShader(wrl::ComPtr<ID3D10Blob> shaderData) {
//	PixelShader* result = new PixelShader();
//	result->pPSData = shaderData;
//	device->CreatePixelShader(shaderData->GetBufferPointer(), shaderData->GetBufferSize(), nullptr, &result->pPixelShader);
//	return result;
//}
//
//VertexShader* D3D11Renderer::CreateVertexShader(wrl::ComPtr<ID3D10Blob> shaderData, const D3D11_INPUT_ELEMENT_DESC* inputLayout, UINT inputLayoutSize) {
//	VertexShader* result = new VertexShader();
//	result->pVSData = shaderData;
//	GFX_THROW_INFO(device->CreateVertexShader(shaderData->GetBufferPointer(), shaderData->GetBufferSize(), nullptr, &result->pVertexShader));
//	GFX_THROW_INFO(device->CreateInputLayout(
//		inputLayout,
//		inputLayoutSize,
//		shaderData->GetBufferPointer(),
//		shaderData->GetBufferSize(),
//		&result->pInputLayout));
//
//
//
//	D3D11_BUFFER_DESC cbd;
//	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	cbd.Usage = D3D11_USAGE_DEFAULT;
//	cbd.CPUAccessFlags = 0u;
//	cbd.MiscFlags = 0u;
//	cbd.ByteWidth = sizeof(result->localBuffer);
//	cbd.StructureByteStride = 0u;
//	GFX_THROW_INFO(device->CreateBuffer(&cbd, NULL, &result->pConstantBuffer));
//
//	return result;
//}
//

PixelShader* D3D11Renderer::CompilePixelShader(void* shaderData, size_t dataSize, ShaderDefines defines[], size_t definesSize, void* includes, const char* enteryPoint, const char* target, uint16_t flags) {
	D3D11PixelShader* result = new D3D11PixelShader();
	std::vector<D3D_SHADER_MACRO> d3ddefines(definesSize + 1);
	for (size_t i = 0; i < definesSize; i++) {
		d3ddefines[i].Definition = defines[i].defenition;
		d3ddefines[i].Name = defines[i].name;
	}
	d3ddefines[definesSize].Definition = NULL;
	d3ddefines[definesSize].Name = NULL;

	wrl::ComPtr<ID3D10Blob>pPSData;
	wrl::ComPtr<ID3D10Blob>psErrorBlob;
#if _DEBUG
	bool compiled = false;
	try {
		GFX_THROW_INFO(D3DCompile(shaderData, dataSize, NULL, d3ddefines.data(), (ID3DInclude*)includes, enteryPoint, target, flags, flags << 8u, &pPSData, &psErrorBlob));
		GFX_THROW_INFO(device->CreatePixelShader(pPSData->GetBufferPointer(), pPSData->GetBufferSize(), nullptr, &result->pPixelShader));
		compiled = true;
	}
	catch (HrException exe) {
		CompileException ce{ __LINE__,  __FILE__, (hr), infoManager.GetMessages(), (char*)psErrorBlob->GetBufferPointer() };
		throw ce;
	}
	catch (InfoException exe) {
		CompileException ce{ __LINE__,  __FILE__, (hr), infoManager.GetMessages(), (char*)psErrorBlob->GetBufferPointer() };
		throw ce;
	}
#else
	GFX_THROW_INFO(D3DCompile(shaderData, dataSize, NULL, d3ddefines.data(), (ID3DInclude*)includes, enteryPoint, target, flags, flags << 8u, &pPSData, &psErrorBlob));
	GFX_THROW_INFO(device->CreatePixelShader(pPSData->GetBufferPointer(), pPSData->GetBufferSize(), nullptr, &result->pPixelShader));
#endif
	return result;
}

VertexShader* D3D11Renderer::CompileVertexShader(void* shaderData, size_t dataSize, ShaderDefines defines[], size_t definesSize, void* includes, const char* enteryPoint, const char* target, uint16_t flags, void* inputLayout, size_t inputLayoutSize) {
	D3D11VertexShader* result = new D3D11VertexShader();

	std::vector<D3D_SHADER_MACRO> d3ddefines(definesSize + 1);
	for (size_t i = 0; i < definesSize; i++) {
		d3ddefines[i].Definition = defines[i].defenition;
		d3ddefines[i].Name = defines[i].name;
	}
	d3ddefines[definesSize].Definition = NULL;
	d3ddefines[definesSize].Name = NULL;

	wrl::ComPtr<ID3D10Blob>pVSData;
	wrl::ComPtr<ID3D10Blob>psErrorBlob;

	const D3D11_INPUT_ELEMENT_DESC* d3dInputLayout = (const D3D11_INPUT_ELEMENT_DESC*)inputLayout;
	//{
	//		{"Position",  0, DXGI_FORMAT_R32G32_FLOAT,  0,                           0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,  0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//};
#if _DEBUG
	bool compiled = false;
	try {
		GFX_THROW_INFO(D3DCompile(shaderData, dataSize, NULL, d3ddefines.data(), (ID3DInclude*)includes, enteryPoint, target, flags, flags << 8u, &pVSData, &psErrorBlob));

		GFX_THROW_INFO(device->CreateVertexShader(pVSData->GetBufferPointer(), pVSData->GetBufferSize(), nullptr, &result->pVertexShader));
		GFX_THROW_INFO(device->CreateInputLayout(
			d3dInputLayout,
			(UINT)inputLayoutSize,
			pVSData->GetBufferPointer(),
			pVSData->GetBufferSize(),
			&result->pInputLayout));
		compiled = true;
	}
	catch (HrException exe) {
		CompileException ce{ __LINE__,  __FILE__, (hr), infoManager.GetMessages(), (char*)psErrorBlob->GetBufferPointer() };
		throw ce;
	}
	catch (InfoException exe) {
		CompileException ce{ __LINE__,  __FILE__, (hr), infoManager.GetMessages(), (char*)psErrorBlob->GetBufferPointer() };
		throw ce;
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


void D3D11Renderer::AddDisposePixelShader(PixelShader* pixelShader) {
	D3D11PixelShader* shader = (D3D11PixelShader*)pixelShader;
	delete pixelShader;
}
void D3D11Renderer::AddDisposeVertexShader(VertexShader* vertexShader) {
	D3D11VertexShader* shader = (D3D11VertexShader*)vertexShader;
	delete vertexShader;
}

void D3D11Renderer::ApplyPixelShader(PixelShader* pixelShader) {
	D3D11PixelShader* shader = (D3D11PixelShader*)pixelShader;
	GFX_THROW_INFO_ONLY(context->PSSetShader(shader->pPixelShader.Get(), nullptr, 0u));
}

void D3D11Renderer::ApplyVertexShader(VertexShader* vertexShader) {
	D3D11VertexShader* shader = (D3D11VertexShader*)vertexShader;
	GFX_THROW_INFO_ONLY(context->VSSetShader(shader->pVertexShader.Get(), nullptr, 0u));
	GFX_THROW_INFO_ONLY(context->IASetInputLayout(shader->pInputLayout.Get()));

	//context->VSSetConstantBuffers(0u, 1u, shader->pConstantBuffer.GetAddressOf());
}

ConstBuffer* D3D11Renderer::CreateConstBuffer(size_t size) {
	D3D11ConstBuffer* result = new D3D11ConstBuffer();

	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.CPUAccessFlags = 0u;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = (UINT)((size / 16 + (size % 16 != 0)) * 16);
	cbd.StructureByteStride = 0u;
	GFX_THROW_INFO(device->CreateBuffer(&cbd, NULL, &result->handle));

	return result;
}

void D3D11Renderer::VerifyConstBuffers(ConstBuffer** constBuffers, size_t size) {

	static std::vector<ID3D11Buffer*> constBuffersArray(16);
	for (size_t i = 0; i < size; i++) {
		constBuffersArray[i] = ((D3D11ConstBuffer*)(constBuffers[i]))->handle.Get();
	}
	GFX_THROW_INFO_ONLY(context->VSSetConstantBuffers(0u, (UINT)size, constBuffersArray.data()));
	GFX_THROW_INFO_ONLY(context->PSSetConstantBuffers(0u, (UINT)size, constBuffersArray.data()));
}

void D3D11Renderer::SetConstBuffer(ConstBuffer* constBuffers, void* data) {
	D3D11ConstBuffer* buffer = (D3D11ConstBuffer*)constBuffers;
	//std::lock_guard<std::mutex> guard(ctxLock);;
	GFX_THROW_INFO_ONLY(context->UpdateSubresource(
		buffer->handle.Get(), 0, 0,
		data, 0, 0));

}

void D3D11Renderer::AddDisposeConstBuffer(ConstBuffer* constBuffers) {
	D3D11ConstBuffer* buffer = (D3D11ConstBuffer*)constBuffers;
	delete buffer;
}

void D3D11Renderer::ApplyPipelineState(PipelineState* piplineState) {

	ApplyPixelShader(piplineState->ps);
	ApplyVertexShader(piplineState->vs);

	SetBlendState(piplineState->bs);
	SetDepthStencilState(piplineState->dss);
	SetBlendFactor(piplineState->bf);
	ApplyRasterizerState(piplineState->rs);
	SetViewport(piplineState->vp);
}


};
