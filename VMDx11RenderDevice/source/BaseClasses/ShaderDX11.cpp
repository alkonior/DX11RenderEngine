#include "pch.h"
#include "ShaderDX11.h"


using namespace GVM;

void ShaderDX11::Place(void* ptr) const
{
    (*static_cast<ShaderDX11*>(ptr)) = *this;
}

ShaderDX11::~ShaderDX11(){}
