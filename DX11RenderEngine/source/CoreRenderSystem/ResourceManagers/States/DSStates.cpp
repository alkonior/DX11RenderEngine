#include "pch.h"
#include "DSStates.h"

using namespace Renderer;

DepthStencilState DepthStencilStates::NoDSS;
DepthStencilState DepthStencilStates::DSS;
DepthStencilState DepthStencilStates::NoWrite;
int  DepthStencilStates::Init()
{
    
    NoDSS.depthBufferEnable = false;
    NoDSS.depthBufferWriteEnable = false;
    NoDSS.depthBufferFunction = CompareFunction::COMPAREFUNCTION_LESSEQUAL;
    NoDSS.stencilEnable = false;
    
    DSS.depthBufferEnable = true;
    DSS.depthBufferWriteEnable = true;
    DSS.depthBufferFunction = CompareFunction::COMPAREFUNCTION_LESSEQUAL;
    DSS.stencilEnable = false;

    
    NoWrite.depthBufferEnable = true;
    NoWrite.depthBufferWriteEnable = false;
    NoWrite.depthBufferFunction = CompareFunction::COMPAREFUNCTION_LESSEQUAL;
    NoWrite.stencilEnable = false;

    
    return 0;
}


int idknfgvasfdsdfvsz = DepthStencilStates::Init();