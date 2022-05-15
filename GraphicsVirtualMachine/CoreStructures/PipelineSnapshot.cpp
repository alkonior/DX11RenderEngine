#include "PipelineSnapshot.h"
using namespace GVM;


const Mesh Mesh::VoidMesh;

uint32_t GVM::PipelineSnapshot::GetSize() {
    uint32_t size = sizeof(Compressed::PipelineSnapshot);

    size += renderTargetsNum * sizeof(RenderTargetDesc::CompressedType);
    size += samplersNum * sizeof(SamplerStateDesc::CompressedType);
    size += viewportsNum * sizeof(ViewportDesc::CompressedType);

    size += constBuffersNum * sizeof(IConstBuffer*);
    size += texturesNum * sizeof(IResourceView*);
    size += mesh.vertexBuffer.buffersCount * sizeof(VertexBufferBinding::CompressedType);
    return size;
}

void GVM::PipelineSnapshot::Compress(void* pointer) {

    using CPS = Compressed::PipelineSnapshot;

    CPS* cps = (CPS*)pointer;

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
    cps->DepthBuffer = DepthBuffer;
    cps->indexBuffer = mesh.indexBuffer;


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
        sizeof(IConstBuffer*) * constBuffersNum);
    pointerPosition +=  sizeof(IConstBuffer*) * constBuffersNum;

    cps->texturesNum = texturesNum;
    memcpy(pointerPosition, Textures,
        sizeof(IResourceView*) * texturesNum);
    pointerPosition +=  sizeof(IConstBuffer*) * constBuffersNum;

    cps->vertexBuffersNum = mesh.vertexBuffer.buffersCount;
    memcpy(pointerPosition, mesh.vertexBuffer.vertexBuffers,
        sizeof(IVertexBuffer*) * mesh.vertexBuffer.buffersCount);
    pointerPosition +=  sizeof(IVertexBuffer*) * mesh.vertexBuffer.buffersCount;
    memcpy(pointerPosition, mesh.vertexBuffer.vertexStride,
        sizeof(uint32_t) * mesh.vertexBuffer.buffersCount);
    pointerPosition +=  sizeof(uint32_t) * mesh.vertexBuffer.buffersCount;
    memcpy(pointerPosition, mesh.vertexBuffer.vertexOffset,
        sizeof(uint32_t) * mesh.vertexBuffer.buffersCount);
    
}
