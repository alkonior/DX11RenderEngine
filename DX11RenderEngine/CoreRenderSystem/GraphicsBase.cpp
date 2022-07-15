#include "GraphicsBase.h"

using namespace Renderer;

PresentationParameters GraphicsBase::GenParams(HWND hWnd1, HWND hWnd2, size_t width, size_t height) {
	PresentationParameters pp;

	pp.isFullScreen = false;
	pp.backBufferWidth = width;
	pp.backBufferHeight = height;
	pp.deviceWindowHandle1 = hWnd1;
	pp.deviceWindowHandle2 = hWnd2;
	pp.multiSampleCount = 0;
	pp.depthStencilFormat = DepthFormat::DEPTHFORMAT_D32;
	pp.presentationInterval = PresentInterval::PRESENTINTERVAL_IMMEDIATE;

	return pp;
}

GraphicsBase::GraphicsBase(InitStruct init)
	:width(init.width), height(init.height), pRenderer(GenParams(init.hWnd1, init.hWnd2, init.width, init.height), 1){

	pLocalConstants = renderer->CreateConstBuffer(sizeof(viewConstants));
	renderer->VerifyConstBuffer(pLocalConstants, mainConstants.slot);
}


void GraphicsBase::SetRenderData(const RenderData& data){
	viewConstants.dt = data.time - time; 
	time = data.time;
	
	viewConstants.past_projection = viewConstants.projection;
	viewConstants.past_inverseProjection = viewConstants.inverseProjection;
	viewConstants.past_view = viewConstants.view;
	viewConstants.past_inverseView = viewConstants.inverseView;
	viewConstants.past_viewProjection = viewConstants.viewProjection;
	viewConstants.past_viewProjectionInverse = viewConstants.viewProjectionInverse;
	

	
	viewConstants.view = data.view.Transpose();
	viewConstants.inverseView = data.view.Invert().Transpose();
	

	viewConstants.projection = data.projection.Transpose();
	viewConstants.inverseProjection = data.projection.Invert().Transpose();

	matrix viewProjection = data.view * data.projection;
	viewConstants.viewProjection = viewProjection.Transpose();
	viewConstants.viewProjectionInverse = viewProjection.Invert().Transpose();
	
	
	viewConstants.reprojectionMatrix    = viewProjection.InvertHighPrecision() * viewConstants.past_viewProjection;
	
	renderer->SetConstBuffer(pLocalConstants, &viewConstants);
	
}

