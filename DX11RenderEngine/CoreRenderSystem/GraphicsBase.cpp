#include "GraphicsBase.h"

using namespace Renderer;

PresentationParameters GraphicsBase::GenParams(HWND hWnd, size_t width, size_t height) {
	PresentationParameters pp;

	pp.isFullScreen = false;
	pp.backBufferWidth = width;
	pp.backBufferHeight = height;
	pp.deviceWindowHandle = hWnd;
	pp.multiSampleCount = 0;
	pp.depthStencilFormat = DepthFormat::DEPTHFORMAT_D32;
	pp.presentationInterval = PresentInterval::PRESENTINTERVAL_IMMEDIATE;

	return pp;
}

GraphicsBase::GraphicsBase(HWND hWnd, size_t width, size_t height)
	:width(width), height(height), renderer(GenParams(hWnd, width, height), 1){

	pLocalConstants = renderer.CreateConstBuffer(sizeof(localConstants));
	renderer.VerifyConstBuffer(pLocalConstants, mainConstants.slot);
}


void GraphicsBase::SetRenderData(const RenderData& data){
	localConstants.dt = data.time - time; 
	time = data.time;
	
	localConstants.past_projection = localConstants.projection;
	localConstants.past_inverseProjection = localConstants.inverseProjection;
	localConstants.past_view = localConstants.view;
	localConstants.past_inverseView = localConstants.inverseView;
	localConstants.view = data.view.Transpose();
	localConstants.inverseView = data.view.Invert().Transpose();

	localConstants.projection = data.projection.Transpose();
	localConstants.inverseProjection = data.projection.Invert().Transpose();
	
	renderer.SetConstBuffer(pLocalConstants, &localConstants);
	
}

