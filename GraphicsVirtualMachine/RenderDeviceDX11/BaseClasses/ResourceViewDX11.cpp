#include "ResourceViewDX11.h"

void ResourceViewDX11::Place(void* ptr) const
{
    (*static_cast<ResourceViewDX11*>(ptr)) = *this;
}

ResourceViewDX11::~ResourceViewDX11(){}
