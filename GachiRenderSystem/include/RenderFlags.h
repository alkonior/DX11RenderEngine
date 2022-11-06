#pragma once


enum ModelDefines {
    MZERO = 0,
    MRED = 1,
    MBAD_UV = 4,
    MNONORMAL = 8,
    MCOLORED = 16,
    MLIGHTED = 32
};

enum DebugPassDefines {
    DZERO = 0,
    DRED = 1,
    D2D = 2,
    D3D = 4,
};

enum  UIDefines : std::uint32_t {
    UIZERO = 0,
    UIRED = 1,
    UICOLORED = 2,
    UISCALED = 4,

    UICHAR = 32
};

enum UPDefines  : uint64_t {
    UPZERO = 0,
    UPRED = 1,
    UPALPHA = 2,
    UPLIGHTMAPPED = 4
};

enum  SkyboxDefines : std::uint32_t {
    SKYZERO = 0,
    SKYRED = 1,
    SKYNOTHING = 2,
};