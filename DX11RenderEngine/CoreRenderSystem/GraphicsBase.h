#pragma once
#include "pch.h"
#include "D3D11Renderer/D3D11Renderer.h"
#include "Utils\TransformUtils.h"
#include "Utils\include/SimpleMath.h"
#include "CoreShaderInclude.h"
#include "Utils\GDIPlusManager.h"
#include "ResourceManagers\TexturesManager.h"
#include "ResourceManagers\ModelsManager.h"
#include "ResourceManagers\BlendStates.h"
#include "ResourceManagers\Samplers.h"

struct RenderData{
	float  time;
	
	matrix view;
	matrix projection;
};

class GraphicsBase {

static Renderer::PresentationParameters GenParams(HWND hWnd, size_t width, size_t height);
GDIPlusManager gpmt;
public:
	GraphicsBase(HWND hWnd, size_t width, size_t height);

	Renderer::D3D11Renderer renderer;

	size_t width;
	size_t height;

	
	uint64_t skyFlags;


	float time;
	void UpdateBaseConstants ();

	MainConstants localConstants;
	Renderer::ConstBuffer* pLocalConstants;
	
	TexturesManager texturesManger;
	ModelsManager modelsManadger;

	void SetRenderData(const RenderData& data);
	

};