#pragma once
#include "CoreRenderSystem/RenderPasses/BaseRenderPass.h"
#include "DebugPassProvider.h"
#include "ResourceManagers/TexturesManager.h"
#include "DebugPassConstBuffer.h"
#include "Utils/DrawData.h"
#include "Utils/ModelMesh.h"

#include "DynamicMeshBuffer.h"
#include "RendererPasses/GachiBasePass.h"

class DebugRenderPass : public GachiBasePass{

    struct DrawCall3D {

        MeshHashData hash;
        Transform world;
        float3 color;
        DebugPassFlags flags;
    };
    
    struct DrawCall2D {
        
        MeshHashData hash;
        float3 color;
        DebugPassFlags flags;
    };	
	
    PipelineFactoryFlags ParseFlags(const DebugPassFlags& flags);
    
public:
    explicit DebugRenderPass(BaseRenderSystem& in);
    
    void Draw(const DebugDraw3DData& drawData);
    void Draw(const DebugDraw2DData& drawData);
    
    void Resize() override;
    void Init(const char* dirr) override;
    void PreRender() override;


    void Render();
    
    void PostRender() override;
    ~DebugRenderPass() override;
    void SetupSettings(const RenderSettings& Settings) override;

private:
    DebugCosntBuffer dataBuffer;

    Renderer::ConstBuffer* pDataCB;
    
    DynamicMeshBuffer<DebugVertex2D> vetexBuffer2D;
    DynamicMeshBuffer<DebugVertex3D> vetexBuffer3D;

    std::vector<DrawCall3D> drawCalls3D;
    std::vector<DrawCall2D> drawCalls2D;
	Renderer::Viewport vp;
};
