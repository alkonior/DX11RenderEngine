#pragma once

#include "IRenderer.h"
#include "TextureData.h"
#include "FloatData.h"
//#include "DrawData.h"

#include <map>

#include "Float3Data.h"
#include "SID.h"



class ITexturesManager {
public:
	Renderer::IRenderer* renderDevice = nullptr;

	struct TextureCache {
		Renderer::Texture* texture = nullptr;
		uint32_t width = 0;
		uint32_t height = 0;
	};

	struct FloatTextureCache {
		Renderer::Texture* texture = nullptr;
		uint32_t width = 0;
		uint32_t height = 0;
	};
	struct Float3TextureCache {
		Renderer::Texture* texture = nullptr;
		uint32_t width = 0;
		uint32_t height = 0;
	};

public:
	ITexturesManager(Renderer::IRenderer* renderDevice): renderDevice(renderDevice){};

#pragma region RGBATextures
	virtual void RegTexture(void* tx, int width, int height, size_t id) = 0;
	virtual void RegTexture(const TextureData& tx, size_t id) = 0;
	
	virtual void UpdateFloatTexture(const TextureData& tx, size_t id) = 0;
	virtual void UpdateFloatTexture(const ImageUpdate& updateData) = 0;
	
	
	virtual void ReleaseTexture(size_t id) = 0;
	virtual void ResizeTextures() = 0;
	virtual TextureCache GetImg(size_t id) = 0;
#pragma endregion

#pragma region FloatTextures
	virtual void RegFloatTexture(float* tx, int width, int height, size_t id) = 0;
	virtual void RegFloatTexture(const FloatData& tx, size_t id) = 0;
	
	virtual void UpdateFloatTexture(const FloatData& tx, size_t id) = 0;
	virtual void UpdateFloatTexture(const FloatImageUpdate& updateData) = 0;
	
	
	virtual void ReleaseFloatTexture(size_t id) = 0;
	virtual FloatTextureCache GetFloatImg(size_t id) = 0;
#pragma endregion 

#pragma region Float3Textures
	virtual void RegFloat3Texture(float3* tx, int width, int height, size_t id) = 0;
	virtual void RegFloat3Texture(const Float3Data& tx, size_t id) = 0;
	
	virtual void UpdateFloat3Texture(const Float3Data& tx, size_t id) = 0;
	virtual void UpdateFloat3Texture(const Float3ImageUpdate& updateData) = 0;
	
	virtual void ReleaseFloat3Texture(size_t id) = 0;
	virtual Float3TextureCache GetFloat3Img(size_t id) = 0;
#pragma endregion 

	struct RenderTargetDescription{
		const char* name = "";
		Renderer::SurfaceFormat format = Renderer::SURFACEFORMAT_COLOR;
		bool isUA = false;
		bool isVS = false;
		bool isSS = false;
		uint32_t width = 0;
		uint32_t height = 0;
	};

	virtual const Renderer::RenderTargetBinding& CreatePublicRenderTarget(
		const RenderTargetDescription& description) = 0;

	virtual const Renderer::RenderTargetBinding& CreatePrivateRenderTarget(
		const RenderTargetDescription& description,
		Renderer::Texture*& diffuseColor,
		Renderer::RenderTargetBinding& diffuseColorRT
	) = 0;

	virtual Renderer::RenderTargetBinding* GetRenderTarget(string_id id) = 0;
	virtual RenderTargetDescription GetRenderTargetDescription(string_id id) = 0;
	virtual std::vector<const char*> GetRenderTargetsList() = 0;
	
	Renderer::Renderbuffer* depthBuffer = nullptr;
	
	
	virtual bool WasIdUsed(size_t id) = 0;
	
	virtual ~ITexturesManager() = default;

	
};

