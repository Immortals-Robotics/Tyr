#pragma once

namespace Tyr::Soccer
{
inline std::optional<Common::RobotState> findNearestOpp(const Common::Vec2 t_pos, const int t_mask,
                                                        const bool t_accept_near_ball = true)
{
    float mdis  = std::numeric_limits<float>::max();
    int   index = -1;
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        if (i == t_mask)
            continue;
        if (State::world().opp_robot[i].seen_state == Common::SeenState::CompletelyOut)
            continue;
        if (Field::isOut(State::world().opp_robot[i].position))
            continue;
        if ((!t_accept_near_ball) &&
            ((State::world().ball.position.distanceTo(State::world().opp_robot[i].position) < 500)))
        {
            continue;
        }
        if ((t_pos.distanceTo(State::world().opp_robot[i].position) < mdis))
        {
            mdis  = t_pos.distanceTo(State::world().opp_robot[i].position);
            index = i;
        }
    }

    if (index == -1)
    {
        return std::nullopt;
    }

    return State::world().opp_robot[index];
}
} // namespace Tyr::Soccer
