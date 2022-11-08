#pragma once
#include <map>
#include "ShadersFlags.h"
#include <memory>

#include <vector>
#include "GBuffer.h"
#include "Lights.h"


struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct CubeMapHelper;
struct ZTexViews;
class SkyBoxGameComponent;
class SkyRenderer;



class RenderingSystemPBR
{
	Game* game;

	ID3D11Buffer* constantBuffer;
	ID3D11Buffer* materialBuffer;
	ID3D11Buffer* constantToneBuffer;
	ID3D11Buffer* lightsBuffer;

	ID3D11SamplerState* samplerWrap;
	ID3D11SamplerState* samplerClamp;
	ID3D11SamplerState* shadowSampler;

	ID3D11DepthStencilState* depthStateOpaque;
	ID3D11DepthStencilState* dsLightingLess;
	ID3D11DepthStencilState* dsLightingGreater;
	ID3D11DepthStencilState* dsNoDepth;

#pragma pack(push, 4)
	struct ConstantStruct
	{
		DirectX::SimpleMath::Matrix World;
		DirectX::SimpleMath::Matrix View;
		DirectX::SimpleMath::Matrix Proj;
		DirectX::SimpleMath::Matrix WorldViewProj;
		DirectX::SimpleMath::Matrix InvertTransposeWorld;
		DirectX::SimpleMath::Vector4 ViewerPos;
	};
#pragma pack(pop)

#pragma pack(push, 4)
	struct MaterialStruct
	{
		float MetallicMult;
		float RoughnessMult;
		DirectX::SimpleMath::Vector2 padding;
	};
#pragma pack(pop)

	ConstantStruct	constData = ConstantStruct{};

	ID3D11RasterizerState*	rastCullBack;
	ID3D11RasterizerState*	rastCullFront;
	ID3D11BlendState*		blendStateOpaque;
	ID3D11BlendState*		blendStateLight;

	std::vector< std::shared_ptr<StaticMesh>>	Meshes;
	std::vector<LightBase*>		Lights;


public:
	GBuffer gBuffer;

	CubeMapHelper* cubeHelper = nullptr;

	ZTexViews* skyMap = nullptr;
	ZTexViews* preMap = nullptr;
	ZTexViews* intMap = nullptr;
	ZTexViews* convMap = nullptr;

	SkyBoxGameComponent* skyBox = nullptr;
	SkyRenderer* skyRend = nullptr;

	
public:
	RenderingSystemPBR(Game* game);

	
#pragma region ShadersPart Opaque
protected:
	ID3D11InputLayout* layoutOpaque			= nullptr;
	ID3D11InputLayout* layoutOpaqueAnimated = nullptr;
	ID3D11InputLayout* layoutLighting		= nullptr;

	std::map<VertexFlagsOpaque, ID3D11VertexShader*>	OpaqueVSs;
	std::map<PixelFlagsOpaque, ID3D11PixelShader*>		OpaquePSs;

	std::map<VertexFlagsLighting, ID3D11VertexShader*>	LightingVSs;
	std::map<PixelFlagsLighting, ID3D11PixelShader*>	LightingPSs;

	std::vector<VertexFlagsOpaque> vsFlagsOpaque = {
		VertexFlagsOpaque::NONE,
		VertexFlagsOpaque::SKINNED_MESH
	};
	
	std::vector<PixelFlagsOpaque> psFlagsOpaque = {
		PixelFlagsOpaque::NONE,
		PixelFlagsOpaque::INVERT_NORMAL
	};

	std::vector<VertexFlagsLighting> vsFlagsLighting = {
		VertexFlagsLighting::NONE,
		VertexFlagsLighting::SCREEN_QUAD
	};
	std::vector<PixelFlagsLighting> psFlagsLighting = {
		PixelFlagsLighting::AmbientLight,
		PixelFlagsLighting::DirectionalLight,
		PixelFlagsLighting::PointLight,
		PixelFlagsLighting::SpotLight,
	};
	
public:
	void ReloadShadersOpaque();
	
#pragma endregion ShadersPart Opaque

#pragma region ShadersPart ToneMapping
public:
	float Luminance = 1.0f;

	std::vector <VSToneMapFlags> vsFlagsTone = {
		VSToneMapFlags::NONE,
	};
	
	std::vector <PSToneMapFlags> psFlagsTone = {
		PSToneMapFlags::NONE,
		PSToneMapFlags::CopyColor,
		PSToneMapFlags::Simple,
		PSToneMapFlags::WithLum,
		PSToneMapFlags::WithLumEx,
	};

	std::map<VSToneMapFlags, ID3D11VertexShader*>	ToneVSs;
	std::map<PSToneMapFlags, ID3D11PixelShader*>	TonePSs;

	PSToneMapFlags ToneFlagPs = PSToneMapFlags::CopyColor;

	void ReloadShadersToneMap();

#pragma endregion ShadersPart ToneMapping

	
public:
	void Game_ScreenResize(const Vector2& newSize);

	void OpaquePass(float deltaTime);
	void LightingPass(float deltaTime);
	void ToneMapPass(float deltaTime);

	void Update(float deltaTime);
	void Draw(float deltaTime);

	void RegisterMesh(StaticMesh* mesh);
	void RegisterMeshes(const std::vector<std::shared_ptr<StaticMesh>>& meshes);
	void UnregisterMesh(std::shared_ptr<StaticMesh> mesh);

	void RegisterLight(LightBase* lBase);
	void UnregisterLight(LightBase* lBase);
};

