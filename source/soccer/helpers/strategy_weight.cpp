#include "../ai.h"

namespace Tyr::Soccer
{
int Ai::strategy_weight()
{
    if (!playBook)
    {
        Common::logError("strategy_weight__+:  444");
        return -1;
    }

    if (playBook->strategy_size() == 0)
    {
        Common::logWarning("strategy_weight__+:  555");
        return -1;
    }

    Common::logDebug("strategy_weight__+:  333");

    std::map<int, float> good_strs;

    float sigma_w = 0;

    Common::logDebug("strategy:");
    for (int i = 0; i < playBook->strategy_size(); i++)
    {
        if ((side * Common::worldState().ball.position.x > playBook->strategy(i).min_x()) &&
            (side * Common::worldState().ball.position.x < playBook->strategy(i).max_x()) &&
            (std::fabs(Common::worldState().ball.position.y) > playBook->strategy(i).min_y()) &&
            (std::fabs(Common::worldState().ball.position.y) < playBook->strategy(i).max_y()) &&
            (playBook->weight(i) > 0))
        {
            Common::logDebug("{}|{}    ", i, playBook->weight(i));

            good_strs[i] = sigma_w + playBook->weight(i);
            sigma_w += playBook->weight(i);
        }
    }

    if (sigma_w == 0)
    {
        return -1;
    }

    int ans_str = -1;
    for (std::map<int, float>::const_iterator it = good_strs.begin(); it != good_strs.end(); ++it)
    {
        if (randomParam * sigma_w <= it->second)
        {
            ans_str = it->first;
            break;
        }
    }

    return ans_str;
}
} // namespace Tyr::Soccer
