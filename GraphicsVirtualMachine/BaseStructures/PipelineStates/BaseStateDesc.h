#pragma once
#include <cstdint>
#include "BaseStructures\GVMEnums.h"

struct BaseStateDesc {
    BaseStateDesc(uint8_t State) : State(State) {}
    uint8_t State;
};
