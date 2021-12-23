#pragma once
#include "pch.h"
#include "Utils.h"

namespace Renderer {

struct D3D11VertexBufferBinding : VertexBufferBinding {
	//FNA3D_VertexDeclaration vertexDeclaration;
	D3D11Buffer vertexBuffer;
	UINT vertexStride;
	UINT vertexOffset;
	//int8_t instanceFrequency;
};

}