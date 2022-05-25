#include "ShaderDX11.h"

void ShaderDX11::Place(void* ptr) const
{
    (*static_cast<ShaderDX11*>(ptr)) = *this;
}

ShaderDX11::~ShaderDX11(){}
