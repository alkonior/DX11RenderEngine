#pragma once
#include "pch.h"
#include "Utils.h"

namespace Renderer {
struct D3D11Buffer : Buffer
{
	wrl::ComPtr<ID3D11Buffer> handle;
	uint8_t dynamic;
	int32_t size;
};

}