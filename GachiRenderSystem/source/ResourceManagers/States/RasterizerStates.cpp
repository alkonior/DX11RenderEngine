#include "RasterizerStates.h"

using namespace Renderer;

RasterizerState RasterizerStates::All;
RasterizerState RasterizerStates::ClockWise;
RasterizerState RasterizerStates::CClockWise;
RasterizerState RasterizerStates::Wireframe;

int RasterizerStates::Init()
{
    ClockWise.cullMode = CullMode::CULLMODE_CULLCLOCKWISEFACE;
    ClockWise.depthBias = 0.0f;
    ClockWise.fillMode = FillMode::FILLMODE_SOLID;
    ClockWise.multiSampleAntiAlias = 0;
    ClockWise.scissorTestEnable = 0;
    ClockWise.slopeScaleDepthBias = 0.0f;
      
    CClockWise.cullMode = CullMode::CULLMODE_CULLCOUNTERCLOCKWISEFACE;
    CClockWise.depthBias = 0.0f;
    CClockWise.fillMode = FillMode::FILLMODE_SOLID;
    CClockWise.multiSampleAntiAlias = 0;
    CClockWise.scissorTestEnable = 0;
    CClockWise.slopeScaleDepthBias = 0.0f;
    
    All.cullMode = CullMode::CULLMODE_NONE;
    All.depthBias = 0.0f;
    All.fillMode = FillMode::FILLMODE_SOLID;
    All.multiSampleAntiAlias = 0;
    All.scissorTestEnable = 0;
    All.slopeScaleDepthBias = 0.0f;
    
    Wireframe.cullMode = CullMode::CULLMODE_NONE;
    Wireframe.depthBias = 0.0f;
    Wireframe.fillMode = FillMode::FILLMODE_WIREFRAME;
    Wireframe.multiSampleAntiAlias = 0;
    Wireframe.scissorTestEnable = 0;
    Wireframe.slopeScaleDepthBias = 0.0f;

    return 0;
}


int issgfbsdfbsdzfvbffn = RasterizerStates::Init();