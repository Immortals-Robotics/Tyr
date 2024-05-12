#include "../ai.h"

namespace Tyr::Soccer
{
int Ai::strategy_weight()
{
    if (m_playbook.strategy_size() == 0)
    {
        Common::logWarning("strategy_weight__+:  555");
        return -1;
    }

    Common::logDebug("strategy_weight__+:  333");

    std::map<int, float> good_strs;

    float sigma_w = 0;

    Common::logDebug("strategy:");
    for (int i = 0; i < m_playbook.strategy_size(); i++)
    {
        if ((side * m_world_state.ball.position.x > m_playbook.strategy(i).min_x()) &&
            (side * m_world_state.ball.position.x < m_playbook.strategy(i).max_x()) &&
            (std::fabs(m_world_state.ball.position.y) > m_playbook.strategy(i).min_y()) &&
            (std::fabs(m_world_state.ball.position.y) < m_playbook.strategy(i).max_y()) && (m_playbook.weight(i) > 0))
        {
            Common::logDebug("{}|{}    ", i, m_playbook.weight(i));

            good_strs[i] = sigma_w + m_playbook.weight(i);
            sigma_w += m_playbook.weight(i);
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
