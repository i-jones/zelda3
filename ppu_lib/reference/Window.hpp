#pragma once

#include "register.hpp"

struct Window
{
    int left;
    int right;
    WindowInOut mode;
    bool enabled;

    bool inWindow(int x) const
    {
        /*
        if (left > right)
        {
            return false;
        }
        */
        bool in = x >= left && x <= right;
        return mode == WindowInOut::In ? in : !in;
    }
};

struct LayerWindow
{
    Window windows[2];
    WindowLogic logic;

    bool isActive(int x) const
    {
        if (windows[0].enabled && windows[1].enabled)
        {
            bool in0 = windows[0].inWindow(x);
            bool in1 = windows[1].inWindow(x);

            switch (logic)
            {
            case WindowLogic::AND:
                return in0 && in1;
            case WindowLogic::OR:
                return in0 || in1;
            case WindowLogic::XOR:
                return in0 ^ in1;
            case WindowLogic::XNOR:
                return !(in0 ^ in1);
            }
        }
        else if (windows[0].enabled)
        {
            return windows[0].inWindow(x);
        }
        else if (windows[1].enabled)
        {
            return windows[1].inWindow(x);
        }
        else
        {
            return false;
        }
    }
};
