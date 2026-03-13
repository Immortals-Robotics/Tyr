#include "../ai.h"
#include "../helpers/open_angle.h"
#include "../skills/kick_ball.h"

#include "../tactics/circle_ball.h"
#include "../tactics/def.h"

namespace Tyr::Soccer
{
void Ai::penaltyUsShootout()
{
    DefTactic{1}.execute(m_own_robot[m_def1]);
    DefTactic{2}.execute(m_own_robot[m_def2]);

    m_own_robot[m_mid5].face(Field::oppGoal());
    m_own_robot[m_mid5].navigate(Common::Vec2(m_side * 4000, 0), VelocityProfile::aroom(),
                                 NavigationFlags::BallObstacle);

    m_own_robot[m_mid1].face(Field::oppGoal());
    m_own_robot[m_mid1].navigate(Common::Vec2(m_side * 4000, -500), VelocityProfile::aroom(),
                                 NavigationFlags::BallObstacle);

    m_own_robot[m_mid2].face(Field::oppGoal());
    m_own_robot[m_mid2].navigate(Common::Vec2(m_side * 4000, 500), VelocityProfile::aroom(),
                                 NavigationFlags::BallObstacle);

    if (m_ref_state.ourPenaltyPrepare() && !m_ref_state.ready)
    {
        CircleBallTactic{Field::oppGoal().angleWith(m_world_state.ball.position), 0, 0}.execute(m_own_robot[m_attack]);
        Common::logInfo("step0 - Waiting for permission");
    }
    else if (m_world_state.ball.position.distanceTo(Field::oppGoal()) > 5500)
    {
        KickBallSkill{Field::oppGoal().angleWith(m_world_state.ball.position), 2700, 0}.execute(m_own_robot[m_attack]);

        Common::logInfo("step1 - Moving forward - waiting to get close to the opp goal");
    }
    else
    {
        OpenAngle open_angle = OpenAngle::calculateOpenAngleToGoal(m_world_state.ball.position, m_own_robot[m_attack]);

        Common::Angle shoot_angle;

        // target the goal center if the open angle is too small
        Common::logCritical("Open angle magnitude {}", open_angle.magnitude.deg());
        if (open_angle.magnitude.deg() > 1.0f)
            shoot_angle = Common::Angle::fromDeg(180.0f) + open_angle.center;
        else
            shoot_angle = (m_world_state.ball.position - Field::oppGoal()).toAngle();

        KickBallSkill{shoot_angle, 5000, 0}.execute(m_own_robot[m_attack]);

        Common::logInfo("step2 - Kick in the goal!!!!");
    }
}
} // namespace Tyr::Soccer
