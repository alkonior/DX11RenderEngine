#pragma once
#include "BaseStructures.h"
#include "IRenderClasses.h"


namespace GVM {

enum RenderDeviceLimitations{
	MAX_RENDERTARGET_ATTACHMENTS = 8,
	MAX_SAMPLER_ATTACHMENTS = 8,
	MAX_TEXTURE_ATTACHMENTS = 8,
	MAX_VIEWPORT_ATTACHMENTS = 8,
	MAX_CONSTBUFFER_ATTACHMENTS = 15,
};

class IRenderDevice {
public:
	IRenderDevice(const IRenderDevice&) = delete;
	IRenderDevice(const IRenderDevice&&) = delete;
	
private:
friend class VirtualMachine;



};


}
