#pragma once
#include "pch.h"
#include "SimpleMath.h"
#include "GraphicsBase.h"
#include "Geometry.h"

#include "Renderers/UIRenderer/UIRenderer.h"
#include "Renderers/ModelRenderer/ModelRenderer.h"
#include "Renderers/IMGUIRenderer/ImGUIRenderer.h"
#include "Renderers/UPRenderer/UPRenderer.h"
#include "Renderers/ParticlesRenderer/ParticlesRenderer.h"
#include "Renderers/SkyboxRenderer/SkyboxRenderer.h"
#include "Renderers/EndRenderer/EndRenderer.h"
#include "Renderers/BloomRenderer/BloomRenderer.h"
#include "Renderers/FXAARenderer/FXAARenderer.h"


#ifdef _DEBUG
#include "GraphicsExceptions/DxgiInfoManager.h"
#endif

namespace sm = DirectX::SimpleMath;

class Graphics:public GraphicsBase{
public:

	Graphics(HWND hWnd, size_t width, size_t height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;


	void BeginFrame();
	bool RenderFrame();
	void EndFrame();
	void ClearBuffer(sm::Vector4 color) noexcept;
	void Flush();



	void RegisterImg(size_t id, const TextureData& text);
	void RegisterImg(size_t id, int width, int heights, void* data, bool mipmap);

	void UpdateImg(size_t id, const TextureData& text);
	void UpdateImg(const ImageUpdate&);
	void ReleaseImg(size_t id);

	void DrawColor(const UIDrawData& data);
	void DrawImg(size_t texId, const UIDrawData& data);


	void RegisterModel(size_t id, const ModelData& model);
	void RegisterFramedModel(size_t id, const FramedModelData& model);
	void ReleaseModel(size_t id);




	void DrawModel(size_t modelId, size_t textureId, Transform position, size_t flags);


	void DrawUserPolygon(MeshHashData model, size_t textureId, UPDrawData data);
	void DrawUserPolygon(MeshHashData model, size_t textureId, size_t lightmapId, UPDrawData data);
	void DrawSetUserPolygon(MeshHashData model, UPModelData newModel, size_t textureId, UPDrawData data);
	MeshHashData RegisterhUserPolygon(UPModelData model, bool dynamic);



	void DrawFramedModel(size_t modelId, size_t textureId, const LerpModelDrawData& data);


	void DrawParticles(const ParticlesMesh& particles, const ParticlesDrawData& data);


	UIRenderer			 manager2D;
	ModelRenderer		 manager3D;
	UPRenderer			 managerUP;
	EndRenderer			 managerEndUP;
	ParticlesRenderer	 managerParticles;
	SkyboxRenderer	     managerSkybox;
	BloomRenderer	     managerBloom;
	ImGUIRenderer	     managerIMGUI;
	FXAARenderer	     managerFXAA;

private:

	enum class DrawCall {
		draw2D,
		draw3D,
		drawUP,
	};

	std::vector<DrawCall> queue;

};