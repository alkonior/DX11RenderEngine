#include "PipelineSnapshot.h"
using namespace GVM;


uint32_t PipelineSnapshot::GetSize(const IStructuresSize& structuresSizes) const {
    uint32_t size = sizeof(Compressed::PipelineSnapshot);

    size += renderTargetsNum * sizeof(RenderTargetDesc::CompressedType);
    
    //size += renderTargetsNum * structuresSizes.IResourceViewSize;
    //size += renderTargetsNum * sizeof(uintptr_t);
    
    size += samplersNum * sizeof(SamplerStateDesc::CompressedType);
    size += viewportsNum * sizeof(ViewportDesc::CompressedType);

    //size += constBuffersNum * structuresSizes.IResourceViewSize;
    //size += texturesNum * structuresSizes.IResourceViewSize;
    
    size += constBuffersNum * sizeof(uintptr_t);
    size += texturesNum * sizeof(uintptr_t);
    
    //size += mesh.vertexBuffer.buffersNum * (structuresSizes.IResourceViewSize);
    
    //size += 6 * structuresSizes.IShaderSize;
    //size += structuresSizes.IInputLayoutSize;
    //size += structuresSizes.IResourceViewSize;//IndexBuffer
    //size += structuresSizes.IResourceViewSize;//DepthBuffer
    
    
    size += mesh.vertexBuffer.buffersNum * (sizeof(uintptr_t));
    size += uaTargetsNum * (sizeof(uintptr_t));
    
    //size += 6 * sizeof(void*);
    //size += sizeof(void*);//InputLayoutSize
    //size += sizeof(void*);//IndexBuffer
    //size += sizeof(void*);//DepthBuffer
    
    return size;
}


void PipelineSnapshot::Compress(const CompressArgs& args) const {
    //using namespace Compressed;
    auto* cps = args.cps;
    
    cps->SnapshotByteSize = GetSize(args.structuresSizes);
    
    
    cps->VS = VS;
    cps->PS = PS;
    cps->CS = CS;
    cps->GS = GS;
    cps->HS = HS;
    cps->DS = DS;

    cps->DrawCallsNum = DrawCallsNum;
    
    cps->primitiveTopology = primitiveTopology;
    cps->rasterizerState = rasterizerState;
    cps->depthStencilState = depthStencilState;
    
    cps->blendDesc = blendDesc;

    auto* pointerPosition = cps->Data;
    cps->indexBuffer = mesh.indexBuffer;
    cps->DepthBuffer = this->DepthStencilBuffer;
    cps->vertexDeclaration = this->InputDeclaration;

    auto renderTargets = (Compressed::RenderTargetDesc*)pointerPosition;
    cps->RenderTargetsShift = pointerPosition - cps->Data;
    cps->renderTargetsNum = renderTargetsNum;
    for (int i =0; i < renderTargetsNum; i++)
    {
        renderTargets[i]= RenderTargets[i];
    }
    pointerPosition+=sizeof(Compressed::RenderTargetDesc)*renderTargetsNum;
    
    auto samplers = (Compressed::SamplerStateDesc*)pointerPosition;
    cps->SamplersShift = pointerPosition - cps->Data;
    cps->samplersNum = samplersNum;
    for (int i =0; i < samplersNum; i++)
    {
        samplers[i] = SamplerStateDesc(Samplers[i]);
    }
    pointerPosition+=sizeof(Compressed::SamplerStateDesc)*samplersNum;
    
    auto viewports = (Compressed::ViewportDesc*)pointerPosition;
    cps->ViewportsShift = pointerPosition - cps->Data;
    cps->viewportsNum = viewportsNum;
    for (int i =0; i < viewportsNum; i++)
    {
        viewports[i] = Viewports[i];
    }
    pointerPosition+=sizeof(Compressed::ViewportDesc)*viewportsNum;
    
    auto constBuffers = (ConstBufferView**)pointerPosition;
    cps->ConstBuffersShift = pointerPosition - cps->Data;
    cps->constBuffersNum = constBuffersNum;
    for (int i =0; i < constBuffersNum; i++)
    {
        constBuffers[i] = ConstBuffers[i];
    }
    pointerPosition+=sizeof(uintptr_t)*constBuffersNum;
    
    auto textures = (ResourceView**)pointerPosition;
    cps->TexturesShift = pointerPosition - cps->Data;
    cps->texturesNum = texturesNum;
    for (int i =0; i < texturesNum; i++)
    {
        textures[i] = Textures[i];
    }
    pointerPosition+=sizeof(uintptr_t)*texturesNum;

    auto vertexBuffers = (VertexBufferView**)pointerPosition;
    cps->VertexBuffersShift = pointerPosition - cps->Data;
    cps->vertexBuffersNum = mesh.vertexBuffer.buffersNum;
    for (int i =0; i < mesh.vertexBuffer.buffersNum; i++)
    {
        vertexBuffers[i] = mesh.vertexBuffer.vertexBuffers[i];
    }
    pointerPosition+=sizeof(uintptr_t)*mesh.vertexBuffer.buffersNum;
    
    auto uaTargets = (UATargetView**)pointerPosition;
    cps->UATargetsShift = pointerPosition - cps->Data;
    cps->uaTargetsNum = uaTargetsNum;
    for (int i =0; i < uaTargetsNum; i++)
    {
        uaTargets[i] =  UATargets[i];
    }
    
    //pointerPosition+=sizeof(uintptr_t)*constBuffersNum;

    /*
    args.resourceManager.GetRealInputLayout(InputDeclaration)->Place(pointerPosition);
    pointerPosition+=sizeof(uintptr_t);
    
    args.resourceManager.GetRealResourceView(DepthStencilBuffer)->Place(pointerPosition);
    pointerPosition+=sizeof(uintptr_t);
    
    args.resourceManager.GetRealResourceView(mesh.indexBuffer)->Place(pointerPosition);
    pointerPosition+=sizeof(uintptr_t);

    
    
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

    */
}

