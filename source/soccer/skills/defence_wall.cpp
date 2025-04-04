#include "defence_wall.h"

#include "../helpers/find_kicker_opp.h"
#include "../robot/robot.h"

namespace Tyr::Soccer
{
const Skill::Id DefenceWallSkill::kId = &DefenceWallSkill::kId;

void DefenceWallSkill::execute(Robot& t_robot)
{
    const float distance = 600.0f + Common::field().robot_radius;

    float x     = -State::side() * State::world().ball.position.x;
    float tetta = -0.000003f * x * x + 0.0016f * x + 90.0f;
    if (m_kick_off)
    {
        tetta = 14.0f;
    }
    Common::logDebug("wall limit: {}", tetta);

    Common::Vec2 target;

    const std::optional<Common::RobotState> opp_attack = findKickerOpp();
    if (opp_attack.has_value())
    {
        Common::logDebug("{}", opp_attack.value().vision_id);

        const Common::Vec2 direction = (State::world().ball.position - opp_attack.value().position).normalized();

        target = State::world().ball.position + direction * distance;
    }
    else
    {
        target = State::world().ball.position.circleAroundPoint(State::world().ball.position.angleWith(Field::ownGoal()), distance);
    }

    Common::Angle ballAngle = State::world().ball.position.angleWith(target);
    Common::Angle firstLeg  = State::world().ball.position.angleWith(
        Common::Vec2(Field::ownGoal().x, Common::sign(State::world().ball.position.y) * (350.0f)));
    Common::Angle secLeg =
        firstLeg - Common::Angle::fromDeg(tetta * Common::sign(State::world().ball.position.y) * State::side());

    Common::logDebug("ball: {}    f: {}    s: {}", ballAngle, firstLeg, secLeg);

    bool isOut =
        false; // std::fabs((std::fabs(NormalizeAngle(ballAngle-firstLeg))+std::fabs(NormalizeAngle(ballAngle-secLeg)))
               // - tetta ) > 1.0f;

    if (isOut)
    {
        target = State::world().ball.position.circleAroundPoint(State::world().ball.position.angleWith(Field::ownGoal()), 730);
    }

    t_robot.face(State::world().ball.position);
    t_robot.navigate(target);
}
} // namespace Tyr::Soccer
