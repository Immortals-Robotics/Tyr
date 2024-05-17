#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::their_place_ball()
{
    GKHi(gk, true);
    DefHi(def, rw, lw, nullptr, true);

    if (Common::setting().mark_in_stop)
    {
        MarkManager(false);

        for (std::map<int *, int>::const_iterator i = markMap.begin(); i != markMap.end(); ++i)
        {
            int opp = i->second;
            int own = *i->first;

            if (opp == -1)
            {
                if (own == dmf)
                {
                    OwnRobot[dmf].face(m_world_state.ball.position);
                    navigate(dmf,
                             m_world_state.ball.position.pointOnConnectingLine(
                                 ownGoal(), m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
                             VelocityProfile::aroom());
                    OwnRobot[dmf].shoot(0);
                }
                else if (own == mid2)
                {
                    OwnRobot[mid2].face(m_world_state.ball.position);
                    navigate(mid2,
                             m_world_state.ball.position.circleAroundPoint(
                                 Common::Angle::fromDeg(-20) + m_world_state.ball.position.angleWith(ownGoal()), 650),
                             VelocityProfile::aroom());
                    OwnRobot[mid2].shoot(0);
                }
                else if (own == mid1)
                {
                    OwnRobot[mid1].face(m_world_state.ball.position);
                    navigate(mid1,
                             m_world_state.ball.position.circleAroundPoint(
                                 Common::Angle::fromDeg(20.0f) + m_world_state.ball.position.angleWith(ownGoal()), 650),
                             VelocityProfile::aroom());
                    OwnRobot[mid1].shoot(0);
                }
                else if (own == attack)
                {
                    OwnRobot[attack].face(m_world_state.ball.position);
                    navigate(attack,
                             m_world_state.ball.position.circleAroundPoint(
                                 m_world_state.ball.position.angleWith(ownGoal()), 650),
                             VelocityProfile::aroom());
                    OwnRobot[attack].shoot(0);
                }
            }
            else
            {
                Mark(own, opp, 500);
            }
        }
    }
    else
    {
        OwnRobot[dmf].face(m_world_state.ball.position);
        navigate(dmf,
                 m_world_state.ball.position.pointOnConnectingLine(
                     ownGoal(), m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
                 VelocityProfile::aroom());

        OwnRobot[rw].face(m_world_state.ball.position);
        navigate(rw,
                 Common::Vec2(0, -100) + m_world_state.ball.position.pointOnConnectingLine(
                                             ownGoal(), m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
                 VelocityProfile::aroom());

        OwnRobot[lw].face(m_world_state.ball.position);
        navigate(lw,
                 Common::Vec2(0, 100) + m_world_state.ball.position.pointOnConnectingLine(
                                            ownGoal(), m_world_state.ball.position.distanceTo(ownGoal()) / 3.0f),
                 VelocityProfile::aroom());

        OwnRobot[mid2].face(m_world_state.ball.position);
        navigate(mid2,
                 m_world_state.ball.position.circleAroundPoint(
                     Common::Angle::fromDeg(-20) + m_world_state.ball.position.angleWith(ownGoal()), 1090),
                 VelocityProfile::aroom());

        OwnRobot[mid1].face(m_world_state.ball.position);
        navigate(mid1,
                 m_world_state.ball.position.circleAroundPoint(
                     Common::Angle::fromDeg(20) + m_world_state.ball.position.angleWith(ownGoal()), 1090),
                 VelocityProfile::aroom());

        OwnRobot[attack].face(m_world_state.ball.position);
        navigate(attack,
                 m_world_state.ball.position.circleAroundPoint(m_world_state.ball.position.angleWith(ownGoal()), 1090),
                 VelocityProfile::aroom());
    }
}
} // namespace Tyr::Soccer
