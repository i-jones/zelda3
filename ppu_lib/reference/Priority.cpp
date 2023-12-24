#include "Priority.hpp"

Priority::Priorities Priority::getPriorities(BgModeReg modeReg)
{
    switch (modeReg.bgMode)
    {
    case BgMode::Mode0:
    case BgMode::Mode1:
    {

        if (modeReg.bg3Priority)
        {
            return Priority::Priorities{
                {3, 6, 9, 12},
                {{8, 11}, {7, 10}, {2, 13}, {1, 4}},
            };
        }
        else
        {
            return Priority::Priorities{
                {3, 6, 9, 12},
                {{8, 11}, {7, 10}, {2, 5}, {1, 4}},
            };
        }
    }
    break;
    case BgMode::Mode2:
    case BgMode::Mode3:
    case BgMode::Mode4:
    case BgMode::Mode5:
    case BgMode::Mode6:
    case BgMode::Mode7:
        return Priority::Priorities{
            {2, 4, 6, 8},
            {{3, 7}, {1, 5}, {-1, -1}, {-1, -1}},
        };
        break;
    default:
        assert(false);
    };
}