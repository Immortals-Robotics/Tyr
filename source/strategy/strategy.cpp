#include "strategy.h"

namespace Tyr::Strategy
{
Strategy::Strategy()
{
    for (int i = 0; i < 8; i++)
    {
        robot[i].waypoint_counter = 0;
    }

    robot[0].name = "gk";
    robot[1].name = "def";
    robot[2].name = "dmf";
    robot[3].name = "lmf";
    robot[4].name = "rmf";
    robot[5].name = "attack";
    robot[6].name = "rw";
    robot[7].name = "lw";

    weight = 0;
}

void Strategy::setName(QString str)
{
    name = str;
}

QString Strategy::getName() const
{
    return name;
}
} // namespace Tyr::Strategy
