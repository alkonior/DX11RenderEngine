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
	:width(width), height(height), renderer(GenParams(hWnd, width, height), 1),
	modelsManadger(&renderer), texturesManger(&renderer) {


		cameraProjection = matrix::CreatePerspective(width, height, 100, 1000);
}

void GraphicsBase::OnResize(size_t width, size_t height) {

}

void GraphicsBase::SetCameraPosition(float3 position) {
	cameraPosition = position;
}

void GraphicsBase::SetCameraMatrix(matrix position) {
	viewMatrix = position;
}

void GraphicsBase::SetCameraProjection(matrix projection) {
	cameraProjection = projection;
}
