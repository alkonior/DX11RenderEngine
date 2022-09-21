#pragma once
#include "TransformUtils.h"
#include "SimpleMath.h"
#include "CoreShaderInclude.h"
#include "DrawData.h"
#include "IRenderer.h"
#include "ResourceManagers\TexturesManager.h"
#include "ResourceManagers\ModelsManager.h"
#include "ResourceManagers\States\BlendStates.h"
#include "ResourceManagers\States\RasterizerStates.h"
#include "ResourceManagers\States\DSStates.h"
#include "ResourceManagers\States\Samplers.h"

class GDIPlusManager;



class GraphicsBase {

static Renderer::PresentationParameters GenParams(const RenderEngineInitStruct& Init);
	GDIPlusManager* gpmt;
public:
	GraphicsBase(RenderEngineInitStruct);

	Renderer::IRenderer* pRenderer;

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
	
	~GraphicsBase();
};