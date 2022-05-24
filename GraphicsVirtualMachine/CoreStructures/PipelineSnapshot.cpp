#include "PipelineSnapshot.h"
using namespace GVM;


const Mesh Mesh::VoidMesh;

uint32_t GVM::PipelineSnapshot::GetSize(const IStructuresSize& structuresSizes) const {
    uint32_t size = sizeof(Compressed::PipelineSnapshot);

    size += renderTargetsNum * (sizeof(RenderTargetDesc::CompressedType)+structuresSizes.IResourceViewSize);
    size += samplersNum * sizeof(SamplerStateDesc::CompressedType);
    size += viewportsNum * sizeof(ViewportDesc::CompressedType);

    size += constBuffersNum * structuresSizes.IResourceViewSize;
    size += texturesNum * structuresSizes.IResourceViewSize;
    size += mesh.vertexBuffer.buffersNum * (sizeof(VertexBufferBinding::CompressedType) + structuresSizes.IResourceViewSize);
    
    size += 6 * structuresSizes.IShaderSize;
    size += structuresSizes.IInputLayoutSize;
    size += structuresSizes.IResourceViewSize;//IndexBuffer
    size += structuresSizes.IResourceViewSize;//DepthBuffer
    
    return size;
}


uint32_t Compressed::PipelineSnapshot::GetSize() const
{
    return SnapshotByteSize;
}

void GVM::PipelineSnapshot::Compress(CompressArgs& args) const {


    
    args.cps->DrawCallsNum = DrawCallsNum;
    args.cps->SnapshotByteSize = GetSize(args.structuresSizes);
    
    args.cps->primitiveType = primitiveType;
    args.cps->rasterizerState = rasterizerState;
    args.cps->depthStencilState = depthStencilState;
    args.cps->blendDesc = blendDesc;



    auto* pointerPosition = args.cps->Data;
    
    
    args.resourceManager.GetRealShader(VS)->Place(pointerPosition);
    pointerPosition+=args.structuresSizes.IShaderSize;
    args.resourceManager.GetRealShader(PS)->Place(pointerPosition);
    pointerPosition+=args.structuresSizes.IShaderSize;
    args.resourceManager.GetRealShader(CS)->Place(pointerPosition);
    pointerPosition+=args.structuresSizes.IShaderSize;
    args.resourceManager.GetRealShader(GS)->Place(pointerPosition);
    pointerPosition+=args.structuresSizes.IShaderSize;
    args.resourceManager.GetRealShader(HS)->Place(pointerPosition);
    pointerPosition+=args.structuresSizes.IShaderSize;
    args.resourceManager.GetRealShader(DS)->Place(pointerPosition);
    pointerPosition+=args.structuresSizes.IShaderSize;


    args.resourceManager.GetRealInputLayout(InputDeclaration)->Place(pointerPosition);
    pointerPosition+=args.structuresSizes.IInputLayoutSize;
    
    args.resourceManager.GetRealResourceView(DepthStencilBuffer)->Place(pointerPosition);
    pointerPosition+=args.structuresSizes.IResourceViewSize;
    
    args.resourceManager.GetRealResourceView(mesh.indexBuffer)->Place(pointerPosition);
    pointerPosition+=args.structuresSizes.IResourceViewSize;

    
    args.cps->vertexBuffersNum = mesh.vertexBuffer.buffersNum;
    for (int i = 0; i < mesh.vertexBuffer.buffersNum; i++)
    {
        args.resourceManager.GetRealResourceView(mesh.vertexBuffer.vertexBuffers[i])->Place(pointerPosition);
        pointerPosition += args.structuresSizes.IResourceViewSize;
    }
    
    memcpy(pointerPosition, mesh.vertexBuffer.vertexStride,
        sizeof(uint32_t) * mesh.vertexBuffer.buffersNum);
    pointerPosition +=  sizeof(uint32_t) * mesh.vertexBuffer.buffersNum;
    memcpy(pointerPosition, mesh.vertexBuffer.vertexOffset,
        sizeof(uint32_t) * mesh.vertexBuffer.buffersNum);
    pointerPosition +=  sizeof(uint32_t) * mesh.vertexBuffer.buffersNum;
    
    
    args.cps->samplersNum = samplersNum;
    auto* samplerPointer = (SamplerStateDesc::CompressedType*)pointerPosition;
    pointerPosition += (samplersNum * sizeof(SamplerStateDesc::CompressedType));
    for (int i = 0; i < samplersNum; i++)
    {
        samplerPointer[i] = Samplers[i].Compress();
    }


    args.cps->renderTargetsNum = renderTargetsNum;
    auto* renderTargetsPointer = (RenderTargetDesc::CompressedType*)pointerPosition;
    for (int i = 0; i < renderTargetsNum; i++)
    {
        renderTargetsPointer[i].BlendState = RenderTargets[i].BlendState;
    }
    pointerPosition += renderTargetsNum*sizeof(RenderTargetDesc::CompressedType);
    
    for (int i = 0; i < renderTargetsNum; i++)
    {
        args.resourceManager.GetRealResourceView(RenderTargets[i].rtv)->Place(pointerPosition);
        pointerPosition += args.structuresSizes.IResourceViewSize;
    }
    
    args.cps->viewportsNum = viewportsNum;
    memcpy(pointerPosition, Viewports,
        sizeof(ViewportDesc) * viewportsNum);
    pointerPosition +=  sizeof(ViewportDesc) * viewportsNum;
    
    args.cps->constBuffersNum = constBuffersNum;
    for (int i = 0; i < constBuffersNum; i++)
    {
        args.resourceManager.GetRealResourceView(ConstBuffers[i])->Place(pointerPosition);
        pointerPosition += args.structuresSizes.IResourceViewSize;
    }

    args.cps->texturesNum = texturesNum;
    for (int i = 0; i < texturesNum; i++)
    {
        args.resourceManager.GetRealResourceView(Textures[i])->Place(pointerPosition);
        pointerPosition += args.structuresSizes.IResourceViewSize;
    }

    
}

