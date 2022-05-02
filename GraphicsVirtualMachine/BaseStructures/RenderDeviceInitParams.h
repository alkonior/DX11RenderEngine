#pragma once
#include "GVMEnums.h"

namespace GVM {

struct PlatformHandle {
#if defined(GVM_LINUX)
	xcb_connection_t*                   connection;
	xcb_window_t                        window;
#elif defined(GVM_MSW)
	HINSTANCE                           hinstance;
	HWND                                hwnd;
#endif
};


struct RenderDeviceInitParams {
	int32_t			backBufferWidth;
	int32_t			backBufferHeight;
	int32_t			multiSampleCount;
	PlatformHandle  deviceWindowHandle;
	PresentInterval presentationInterval;
};


}
