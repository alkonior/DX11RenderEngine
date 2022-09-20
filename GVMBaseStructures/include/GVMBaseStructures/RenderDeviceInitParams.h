#pragma once
#include "GVMEnums.h"

namespace GVM {

struct PlatformHandle {
};


struct RenderDeviceInitParams {
	int32_t			 backBufferWidth;
	int32_t			 backBufferHeight;
	//int32_t			 multiSampleCount;
	PlatformHandle*   deviceWindowHandle;
	EPresentInterval presentationInterval;
};


}
