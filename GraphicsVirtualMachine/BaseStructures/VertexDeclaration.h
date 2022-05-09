#pragma once
#include <cstdint>
#include <vector>
#include "GVMEnums.h"

namespace GVM {

struct VertexDeclarationDesc {
    inline static constexpr uint32_t APPEND_ALIGNED_ELEMENT = ( 0xffffffff );
    
    const char*                SemanticName;
    EFormat                Format;
    uint8_t                    InputSlot;
    uint8_t                    SemanticIndex = 0;
    uint32_t                   AlignedByteOffset = APPEND_ALIGNED_ELEMENT;
    EInputClassification       InputSlotClass = EInputClassification::INPUT_PER_VERTEX_DATA;
    uint32_t                   InstanceDataStepRate = 0;
};

struct InputAssemblerDeclarationDesc {
    std::vector<VertexDeclarationDesc> InputElementDescs;
};

}