#pragma once

#include <array>

#include "register.hpp"

class Priority
{
public:
    using BGPriorities = std::array<int, 2>;
    using ObjPriorities = std::array<int, 4>;
    static constexpr int BasePriority = 0;

    struct Priorities
    {
        ObjPriorities obj;
        BGPriorities bg[4];
    };

    static Priorities getPriorities(BgModeReg modeReg);
};