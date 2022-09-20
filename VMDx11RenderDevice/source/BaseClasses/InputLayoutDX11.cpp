#include "pch.h"
#include "InputLayoutDX11.h"


using namespace GVM;


void InputLayoutDX11::Place(void* ptr) const
{
    (*static_cast<InputLayoutDX11*>(ptr)) = *this;
}

InputLayoutDX11::~InputLayoutDX11(){}
