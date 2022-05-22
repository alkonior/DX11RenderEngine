#include "PipelineSnapshot.h"
using namespace GVM;


const Mesh Mesh::VoidMesh;

uint32_t GVM::PipelineSnapshot::GetSize() const {
    uint32_t size = sizeof(Compressed::PipelineSnapshot);

    size += renderTargetsNum * sizeof(RenderTargetDesc::CompressedType);
    size += samplersNum * sizeof(SamplerStateDesc::CompressedType);
    size += viewportsNum * sizeof(ViewportDesc::CompressedType);

    size += constBuffersNum * sizeof(ConstBuffer*);
    size += texturesNum * sizeof(ResourceView*);
    size += mesh.vertexBuffer.buffersNum * sizeof(VertexBufferBinding::CompressedType);
    return size;
}


uint32_t Compressed::PipelineSnapshot::GetSize() const
{
    uint32_t size = sizeof(Compressed::PipelineSnapshot);

    size += renderTargetsNum * sizeof(RenderTargetDesc);
    size += samplersNum * sizeof(SamplerStateDesc);
    size += viewportsNum * sizeof(ViewportDesc);

    size += constBuffersNum * sizeof(ConstBuffer*);
    size += texturesNum * sizeof(ResourceView*);
    size += vertexBuffersNum * sizeof(VertexBufferBinding);
    return size;
}

void GVM::PipelineSnapshot::Compress(Compressed::PipelineSnapshot* cps, const ResourcesManager&) const {


    cps->VS = VS;
    cps->PS = PS;
    cps->CS = CS;
    cps->GS = GS;
    cps->HS = HS;
    cps->DS = DS;
    
    cps->DrawCallsNum = DrawCallsNum;

    cps->primitiveType = primitiveType;
    cps->rasterizerState = rasterizerState.ToUInt();
    cps->depthStencilState = depthStencilState.ToUInt();
    cps->vertexDeclaration = vertexDeclaration;
    cps->DepthBuffer = DepthStencilBuffer;
    cps->indexBuffer = mesh.indexBuffer;
    cps->blendDesc = blendDesc;


    cps->samplersNum = samplersNum;

    auto* pointerPosition = cps->Data;
    auto* samplerPointer = (SamplerStateDesc::CompressedType*)pointerPosition;
    pointerPosition += (samplersNum * sizeof(SamplerStateDesc::CompressedType));
    for (int i = 0; i < samplersNum; i++)
    {
        samplerPointer[i] = Samplers[i].Compress();
    }


    cps->renderTargetsNum = renderTargetsNum;
    auto* renderTargetsPointer = (RenderTargetDesc::CompressedType*)pointerPosition;
    for (int i = 0; i < renderTargetsNum; i++)
    {
        renderTargetsPointer[i].rtv = RenderTargets[i].rtv;
        renderTargetsPointer[i].BlendState = RenderTargets[i].BlendState.ToUInt();
    }
    pointerPosition +=  renderTargetsNum*sizeof(RenderTargetDesc::CompressedType);

    cps->viewportsNum = viewportsNum;
    memcpy(pointerPosition, Viewports,
        sizeof(ViewportDesc) * viewportsNum);
    pointerPosition +=  sizeof(ViewportDesc) * viewportsNum;
    
    cps->constBuffersNum = constBuffersNum;
    memcpy(pointerPosition, ConstBuffers,
        sizeof(ConstBuffer*) * constBuffersNum);
    pointerPosition +=  sizeof(ConstBuffer*) * constBuffersNum;

    cps->texturesNum = texturesNum;
    memcpy(pointerPosition, Textures,
        sizeof(ResourceView*) * texturesNum);
    pointerPosition +=  sizeof(ConstBuffer*) * constBuffersNum;

    cps->vertexBuffersNum = mesh.vertexBuffer.buffersNum;
    memcpy(pointerPosition, mesh.vertexBuffer.vertexBuffers,
        sizeof(VertexBuffer*) * mesh.vertexBuffer.buffersNum);
    pointerPosition +=  sizeof(VertexBuffer*) * mesh.vertexBuffer.buffersNum;
    memcpy(pointerPosition, mesh.vertexBuffer.vertexStride,
        sizeof(uint32_t) * mesh.vertexBuffer.buffersNum);
    pointerPosition +=  sizeof(uint32_t) * mesh.vertexBuffer.buffersNum;
    memcpy(pointerPosition, mesh.vertexBuffer.vertexOffset,
        sizeof(uint32_t) * mesh.vertexBuffer.buffersNum);
    
}

