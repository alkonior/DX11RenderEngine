#include "PipelineSnapshot.h"
using namespace GVM;


const Mesh Mesh::VoidMesh;

uint32_t GVM::PipelineSnapshot::GetSize(const IStructuresSize& structuresSizes) const {
    uint32_t size = sizeof(Compressed::PipelineSnapshot);

    size += renderTargetsNum * sizeof(RenderTargetDesc::CompressedType);
    size += samplersNum * sizeof(SamplerStateDesc::CompressedType);
    size += viewportsNum * sizeof(ViewportDesc::CompressedType);

    size += constBuffersNum * structuresSizes.IResourceViewSize;
    size += texturesNum * structuresSizes.IResourceViewSize;
    size += mesh.vertexBuffer.buffersNum * (sizeof(VertexBufferBinding::CompressedType) + structuresSizes.IResourceViewSize);
    
    size += 6 * structuresSizes.IShaderSize;
    size += structuresSizes.IInputLayoutSize;
    size += structuresSizes.IResourceViewSize;//IndexBuffer
    
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
    args.cps->rasterizerState = rasterizerState.ToUInt();
    args.cps->depthStencilState = depthStencilState.ToUInt();
    args.cps->DepthBuffer = DepthStencilBuffer;
    args.cps->blendDesc = blendDesc;



    auto* pointerPosition = args.cps->Data;
    
    
    ///args.cps->VS =
    args.resourceManager.GetRealShader(VS)->Place(pointerPosition);
    pointerPosition+=args.structuresSizes.IShaderSize;
    
    args.cps->PS = PS;
    args.cps->CS = CS;
    args.cps->GS = GS;
    args.cps->HS = HS;
    args.cps->DS = DS;


    args.cps->vertexDeclaration = vertexDeclaration;
    args.cps->indexBuffer = mesh.indexBuffer;
    
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
        renderTargetsPointer[i].rtv = RenderTargets[i].rtv;
        renderTargetsPointer[i].BlendState = RenderTargets[i].BlendState.ToUInt();
    }
    pointerPosition +=  renderTargetsNum*sizeof(RenderTargetDesc::CompressedType);

    args.cps->viewportsNum = viewportsNum;
    memcpy(pointerPosition, Viewports,
        sizeof(ViewportDesc) * viewportsNum);
    pointerPosition +=  sizeof(ViewportDesc) * viewportsNum;
    
    args.cps->constBuffersNum = constBuffersNum;
    memcpy(pointerPosition, ConstBuffers,
        sizeof(ConstBuffer*) * constBuffersNum);
    pointerPosition +=  sizeof(ConstBuffer*) * constBuffersNum;

    args.cps->texturesNum = texturesNum;
    memcpy(pointerPosition, Textures,
        sizeof(ResourceView*) * texturesNum);
    pointerPosition +=  sizeof(ConstBuffer*) * constBuffersNum;

    args.cps->vertexBuffersNum = mesh.vertexBuffer.buffersNum;
    memcpy(pointerPosition, mesh.vertexBuffer.vertexBuffers,
        sizeof(VertexBuffer*) * mesh.vertexBuffer.buffersNum);
    pointerPosition +=  sizeof(VertexBuffer*) * mesh.vertexBuffer.buffersNum;
    memcpy(pointerPosition, mesh.vertexBuffer.vertexStride,
        sizeof(uint32_t) * mesh.vertexBuffer.buffersNum);
    pointerPosition +=  sizeof(uint32_t) * mesh.vertexBuffer.buffersNum;
    memcpy(pointerPosition, mesh.vertexBuffer.vertexOffset,
        sizeof(uint32_t) * mesh.vertexBuffer.buffersNum);
    
}

