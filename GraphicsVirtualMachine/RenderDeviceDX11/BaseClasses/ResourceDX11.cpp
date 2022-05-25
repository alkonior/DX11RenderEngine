#include "ResourceDX11.h"

void ResourceDX11::Place(void* ptr) const
{
    (*static_cast<ResourceDX11*>(ptr)) = *this;
}

ResourceDX11::~ResourceDX11(){}
