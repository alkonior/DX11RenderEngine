#pragma once
#include <cstdint>

struct BaseStateDesc {
    BaseStateDesc(uint8_t State) : State(State) {}
    uint8_t State;
};
