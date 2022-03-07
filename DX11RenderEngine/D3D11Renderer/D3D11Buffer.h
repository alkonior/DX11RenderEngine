#pragma once
#include "pch.h"
#include "IRenderer/Utils.h"

namespace Renderer {


struct D3D11Buffer : Buffer
{
	wrl::ComPtr<ID3D11Buffer> handle = nullptr;
	uint8_t dynamic = 0;
	int32_t size = 0;
};

struct D3D11ConstBuffer : ConstBuffer
{
	wrl::ComPtr<ID3D11Buffer> handle = nullptr;
	int32_t size = 0;
};
}