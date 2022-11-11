#pragma once
#include "CoreRenderSystem/RenderPasses/BaseRenderPass.h"
#include "RenderSettings.h"

class GachiBasePass : public BaseRenderPass
{
public:
    explicit GachiBasePass(const BaseRendererParams& in)
        : BaseRenderPass(in)
    {
    }

    virtual void SetupSettings(const RenderSettings& Settings) = 0;
    
};
