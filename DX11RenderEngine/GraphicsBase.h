#pragma once
#include "pch.h"
#include "D3D11Renderer.h"
#include "TransformUtils.h"
#include "SimpleMath.h"


class GraphicsBase {

static Renderer::PresentationParameters GenParams(HWND hWnd, size_t width, size_t height);

public:
	GraphicsBase(HWND hWnd, size_t width, size_t height);
	void OnResize(size_t width, size_t height);

	Renderer::D3D11Renderer renderer;

	size_t width;
	size_t height;

	matrix camera;
	matrix cameraProjection;

	void SetCameraPosition(Transform position);
	void SetCameraProjection(matrix projection);
	void SetCameraMatrix(matrix position);

};