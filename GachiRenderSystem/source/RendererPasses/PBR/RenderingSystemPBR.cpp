#include "RenderingSystemPBR.h"

#include "DebugRenderSystem.h"
#include "InputDevice.h"
#include "SkyRenderer.h"
#include "../GameFramework/Game.h"
#include "../GameFramework/Camera.h"
#include "../GameFramework/CubeMapHelper.h"
#include "../GameFramework/StaticMesh.h"
#include "../GameFramework/SkyBoxComp.h"



RenderingSystemPBR::RenderingSystemPBR(Game* game) : game(game), gBuffer(game->Display->ClientWidth, game->Display->ClientHeight)
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MaxLOD = INT_MAX;
	//samplerDesc.MaxAnisotropy = 1.0f;

	game->Device->CreateSamplerState(&samplerDesc, &samplerWrap);

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	game->Device->CreateSamplerState(&samplerDesc, &samplerClamp);

	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;

	game->Device->CreateSamplerState(&samplerDesc, &shadowSampler);
	
	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	game->Device->CreateRasterizerState(&rastDesc, &rastCullBack);
	
	rastDesc.CullMode = D3D11_CULL_FRONT;
	game->Device->CreateRasterizerState(&rastDesc, &rastCullFront);

	auto blendStateDesc = D3D11_BLEND_DESC{ false, false };
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	blendStateDesc.RenderTarget[0].BlendEnable = true;
	blendStateDesc.RenderTarget[0].BlendOp		= D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].BlendOpAlpha	= D3D11_BLEND_OP_ADD;
	
	blendStateDesc.RenderTarget[0].SrcBlend		= D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlend	= D3D11_BLEND_ZERO;
	
	blendStateDesc.RenderTarget[0].SrcBlendAlpha	= D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha	= D3D11_BLEND_ZERO;

	game->Device->CreateBlendState(&blendStateDesc, &blendStateOpaque);

	blendStateDesc.RenderTarget[0].SrcBlend		= D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend	= D3D11_BLEND_ONE;
	game->Device->CreateBlendState(&blendStateDesc, &blendStateLight);
	

	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;
	bufDesc.ByteWidth = sizeof(ConstantStruct);

	game->Device->CreateBuffer(&bufDesc, nullptr, &constantBuffer);

	bufDesc.ByteWidth = sizeof(MaterialStruct);
	game->Device->CreateBuffer(&bufDesc, nullptr, &materialBuffer);


	bufDesc.ByteWidth = sizeof(float) * 4;
	game->Device->CreateBuffer(&bufDesc, nullptr, &constantToneBuffer);
	
	bufDesc.ByteWidth = sizeof(LightData);
	game->Device->CreateBuffer(&bufDesc, nullptr, &lightsBuffer);


	ReloadShadersOpaque();
	ReloadShadersToneMap();


	game->ScreenResized.AddRaw(this, &RenderingSystemPBR::Game_ScreenResize);
	
	auto depthDesc = D3D11_DEPTH_STENCIL_DESC{};
	depthDesc.DepthEnable = true;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.StencilEnable = false;
	depthDesc.StencilReadMask = 0x00;
	depthDesc.StencilWriteMask = 0x00;

	game->Device->CreateDepthStencilState(&depthDesc, &depthStateOpaque);

	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	game->Device->CreateDepthStencilState(&depthDesc, &dsLightingLess);

	depthDesc.DepthFunc = D3D11_COMPARISON_GREATER;
	game->Device->CreateDepthStencilState(&depthDesc, &dsLightingGreater);

	depthDesc.DepthEnable = false;
	game->Device->CreateDepthStencilState(&depthDesc, &dsNoDepth);


	cubeHelper = new CubeMapHelper();
	cubeHelper->Init(game);

	skyMap = new ZTexViews();
	preMap = new ZTexViews();
	convMap = new ZTexViews();
	intMap = new ZTexViews();

	//cubeHelper->ConvertHDR(L"./Content/14-Hamarikyu_Bridge_B_3k.hdr", *skyMap, true);
	cubeHelper->LoadFromDDS(L"./Content/14-Hamarikyu_Bridge_B_3k.dds", *skyMap);
	cubeHelper->LoadFromDDS(L"./Content/HdrMaps/japanHdrDiffuseHDR.dds", *convMap);

	cubeHelper->PreFilterMipMaps(*skyMap, *preMap);
	cubeHelper->GenerateIntegratedBRDFMap(*intMap);
	//cubeHelper->SaveToDDS(intMap->Tex, L"./Content/test.dds");

	//cubeHelper->LoadFromDDS()

	skyBox = new SkyBoxGameComponent(game, game->GameCamera);
	skyBox->Initialize();

	skyBox->cubeMap = skyMap->Tex;
	skyBox->SRV = skyMap->Srv;

	skyRend = new SkyRenderer(game);
	skyRend->BackBufferResized(game->Display->ClientWidth, game->Display->ClientHeight, 1);
	skyRend->Init();
	skyRend->Update(1);
}


void RenderingSystemPBR::Game_ScreenResize(const Vector2& newSize)
{
	gBuffer.Resize(static_cast<int>(newSize.x), static_cast<int>(newSize.y));
}


void RenderingSystemPBR::Update(float deltaTime)
{
	constData.View = game->GameCamera->ViewMatrix;
	constData.Proj = game->GameCamera->ProjMatrix;

	const auto camPos = game->GameCamera->GetPosition();
	constData.ViewerPos = Vector4(camPos.x, camPos.y, camPos.z, 1.0f);

	if(game->InputDevice->IsKeyDown(Keys::R))
	{
		cubeHelper->ReloadShaders();
		cubeHelper->PreFilterMipMaps(*skyMap, *preMap);
		skyBox->cubeMap = preMap->Tex;
		skyBox->SRV = preMap->Srv;
	}

	//game->DebugRender->DrawTextureOnScreen(gBuffer.NormalSRV, 10, 10, 128, 128);
	
	//skyBox->Update(deltaTime);
	skyRend->Update(deltaTime);
}


void RenderingSystemPBR::Draw(float deltaTime)
{
	game->Context->ClearState();

	OpaquePass(deltaTime);
	LightingPass(deltaTime);
	ToneMapPass(deltaTime);

	
	//var fromRes = gBuffer.AccumulationSRV.Resource;
	//var destRes = game.RenderView.Resource;
	//game.Context.CopyResource(fromRes, destRes);
	//
	//fromRes.Dispose();
	//destRes.Dispose();
	//
	//game.Context.ClearState();
	//game.RestoreTargets();
}


void RenderingSystemPBR::RegisterMesh(StaticMesh* mesh)
{
	Meshes.emplace_back(mesh);
}


void RenderingSystemPBR::RegisterMeshes(const std::vector<std::shared_ptr<StaticMesh>>& meshes)
{
	Meshes.insert(Meshes.end(), meshes.begin(), meshes.end());
}


void RenderingSystemPBR::UnregisterMesh(std::shared_ptr<StaticMesh> mesh)
{
	auto new_end = std::remove(Meshes.begin(), Meshes.end(), mesh);
}


void RenderingSystemPBR::RegisterLight(LightBase* lBase)
{
	Lights.emplace_back(lBase);
}


void RenderingSystemPBR::UnregisterLight(LightBase* lBase)
{
	auto new_end = std::remove(Lights.begin(), Lights.end(), lBase);
}


