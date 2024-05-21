#include "../ai.h"

namespace Tyr::Soccer
{
int Ai::strategyWeight()
{
    if (m_playbook.strategies.size() == 0)
    {
        Common::logWarning("strategy_weight__+:  555");
        return -1;
    }

    Common::logDebug("strategy_weight__+:  333");

    std::map<int, float> good_strs;

    float sigma_w = 0;

    const Common::Vec2 sign_modifier{(float) m_side, Common::sign(-m_world_state.ball.position.y)};

    Common::logDebug("strategy:");

    for (int i = 0; i < m_playbook.strategies.size(); i++)
    {
        if ((m_side * m_world_state.ball.position.x > m_playbook.strategies[i].area.min.x) &&
            (m_side * m_world_state.ball.position.x < m_playbook.strategies[i].area.max.x) &&
            (std::fabs(m_world_state.ball.position.y) > m_playbook.strategies[i].area.min.y) &&
            (std::fabs(m_world_state.ball.position.y) < m_playbook.strategies[i].area.max.y) &&
            (m_playbook.weights[i] > 0))
        {
            Common::logDebug("{}|{}    ", i, m_playbook.weights[i]);

            good_strs[i] = sigma_w + m_playbook.weights[i];
            sigma_w += m_playbook.weights[i];
        }
    }

    if (sigma_w == 0)
    {
        return -1;
    }

    int ans_str = -1;
    for (std::map<int, float>::const_iterator it = good_strs.begin(); it != good_strs.end(); ++it)
    {
        if (m_random_param * sigma_w <= it->second)
        {
            ans_str = it->first;
            break;
        }
    }

    return ans_str;
}
} // namespace Tyr::Soccer
