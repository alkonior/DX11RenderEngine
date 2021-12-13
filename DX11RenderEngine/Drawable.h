#pragma once
#include "pch.h"
#include "GraphicsBase.h"

class Drawable {
public:
	
	virtual void Present(GraphicsBase&) = 0;

	virtual ~Drawable() {};
};