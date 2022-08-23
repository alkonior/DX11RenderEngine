#pragma once
#include "pch.h"
#include "D3D11Renderer/D3D11Renderer.h"
#include "Utils\TransformUtils.h"
#include "Utils\include/SimpleMath.h"
#include "CoreShaderInclude.h"
#include "Utils\GDIPlusManager.h"
#include "ResourceManagers\TexturesManager.h"
#include "ResourceManagers\ModelsManager.h"
#include "ResourceManagers\States\BlendStates.h"
#include "ResourceManagers\States\RasterizerStates.h"
#include "ResourceManagers\States\DSStates.h"
#include "ResourceManagers\States\Samplers.h"

struct RenderData{
	float  time;
	
	matrix view;
	matrix projection;
};

struct  InitStruct {
	HWND hWnd1;
	HWND hWnd2;
	size_t width; size_t height;
};

class GraphicsBase {

static Renderer::PresentationParameters GenParams(HWND hWnd1, HWND hWnd2, size_t width, size_t height);
GDIPlusManager gpmt;
public:
	GraphicsBase(InitStruct);

	Renderer::D3D11Renderer pRenderer;

	size_t width;
	size_t height;

	
	uint64_t skyFlags;


	float time;

	TAAShiftBuffer taaConstants;
	MainConstants viewConstants;
	
	Renderer::ConstBuffer* pLocalConstants;
	TexturesManager texturesManger;
	ModelsManager modelsManadger;

	void SetRenderData(const RenderData& data);
	

};