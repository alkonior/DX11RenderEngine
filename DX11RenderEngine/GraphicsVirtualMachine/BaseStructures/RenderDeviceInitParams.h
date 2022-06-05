#pragma once
#include "GVMEnums.h"

namespace GVM {

enum class EDeviceToUse
{
	DX11,
};

struct PlatformHandle {
#if defined(GVM_LINUX)
	//xcb_connection_t*                   connection;
	xcb_window_t                        window;
#elif defined(GVM_MSW)
	//HINSTANCE                           hinstance;
	HWND                                hwnd;
#endif
};


struct RenderDeviceInitParams {
	EDeviceToUse     device;
	int32_t			 backBufferWidth;
	int32_t			 backBufferHeight;
	//int32_t			 multiSampleCount;
	PlatformHandle   deviceWindowHandle;
	EPresentInterval presentationInterval;
};


}
