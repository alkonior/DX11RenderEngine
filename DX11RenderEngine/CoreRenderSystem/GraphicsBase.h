#pragma once
#include "pch.h"
#include "D3D11Renderer/D3D11Renderer.h"
#include "Utils/TransformUtils.h"
#include "SimpleMath.h"

#include "Utils/GDIPlusManager.h"
#include "ResourceManagers/TexturesManager.h"
#include "ResourceManagers/ModelsManager.h"

#ifdef _DEBUG
#define USE_PIX_ON_ALL_ARCHITECTURES
#include "pix3.h"
#endif
class GraphicsBase {

static Renderer::PresentationParameters GenParams(HWND hWnd, size_t width, size_t height);
GDIPlusManager gpmt;
public:
	GraphicsBase(HWND hWnd, size_t width, size_t height);
	void OnResize(size_t width, size_t height);

	Renderer::D3D11Renderer renderer;

	size_t width;
	size_t height;

	float3 cameraPosition;
	matrix viewMatrix;
	matrix cameraProjection;
	uint64_t skyFlags;




	TexturesManager texturesManger;
	ModelsManager modelsManadger;

	void SetCameraPosition(float3 position);
	void SetCameraProjection(matrix projection);
	void SetCameraMatrix(matrix position);

};