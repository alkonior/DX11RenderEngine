#pragma once
#include "TransformUtils.h"
#include "SimpleMath.h"
#include "CoreShaderInclude.h"
#include "IRenderer.h"
#include "ResourceManagers/IModelsManager.h"
#include "ResourceManagers/ITexturesManager.h"


class GDIPlusManager;


class BaseRenderSystem {

	GDIPlusManager* gpmt;
	
protected:
	
	struct BaseRenderSystemInitStruct {
		Renderer::IRenderer* pRenderer;
		ITexturesManager* texturesManger;
		IModelsManager<>* modelsManager;
		
	};
	
	ITexturesManager* texturesManger;
	IModelsManager<>* modelsManager;
	
	BaseRenderSystem(const BaseRenderSystemInitStruct&);

private:
	
	CoreConstants viewConstants;
	Renderer::ConstBuffer* pLocalConstants;

public:
	Renderer::IRenderer* pRenderer;
	
	const CoreConstants& GetViewConstants() {return viewConstants;};
	
	virtual bool Present();
	
	void SetRenderData(const RenderData& data);
	
	virtual ~BaseRenderSystem();
};