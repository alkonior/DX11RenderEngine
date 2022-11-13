#pragma once

//#include "winHandler.h"
#include "IRenderer/Utils.h"

namespace Renderer {


struct D3D11Buffer : Buffer
{
	//wrl::ComPtr<ID3D11Buffer> handle = nullptr;
	union {
		GVM::Resource* handleTest = nullptr;
		GVM::VertexBuffer* vertexTest;
		GVM::IndexBuffer* indexTest;
	};
	GVM::VertexBufferView* vertexViewTest = nullptr;
	GVM::ResourceView* indexViewTest = nullptr;
	uint8_t dynamic = 0;
	int32_t size = 0;
};

struct D3D11ConstBuffer : ConstBuffer
{
	GVM::ConstBuffer* buffer;
	GVM::ConstBufferView* bufferView;
	//wrl::ComPtr<ID3D11Buffer> handle = nullptr;
	int32_t size = 0;
};
}