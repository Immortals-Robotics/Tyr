#include "robot.h"

namespace Tyr::Soccer
{
Common::Vec2 vdes_ann;

Common::Vec3 Robot::MotionPlan(const Common::RobotState &state, const Common::RobotState &target, float speed,
                               const VelocityProfile &velocityProfile)
{
    Common::Vec2 max_spd   = velocityProfile.max_spd;
    Common::Vec2 max_dec   = velocityProfile.max_dec;
    Common::Vec2 max_acc   = velocityProfile.max_acc;
    float        max_w_acc = velocityProfile.max_w_acc;
    float        max_w_dec = velocityProfile.max_w_dec;

    static Common::Vec3 oldAns[12] = {Common::Vec3(0.0f), Common::Vec3(0.0f), Common::Vec3(0.0f), Common::Vec3(0.0f),
                                      Common::Vec3(0.0f), Common::Vec3(0.0f), Common::Vec3(0.0f), Common::Vec3(0.0f),
                                      Common::Vec3(0.0f), Common::Vec3(0.0f), Common::Vec3(0.0f), Common::Vec3(0.0f)};

    Common::Vec3 ans; // The output of this function
    ans.z = target.angle.deg();
    ans.z /= 5.0f;

    Common::Vec2 diff = target.position - state.position;

    Common::Vec2 tmp_max_speed;

    float target_dis = Common::Vec2(0.0f).distanceTo(diff);
    tmp_max_speed.x  = max_spd.x * std::fabs(diff.x) / target_dis;

    tmp_max_speed.y = max_spd.y * std::fabs(diff.y) / target_dis;

    float tmp_vel_mag = Common::Vec2(0.0f).distanceTo(tmp_max_speed);

    if (tmp_vel_mag > speed)
    {
        tmp_max_speed.x *= speed / tmp_vel_mag;
        tmp_max_speed.y *= speed / tmp_vel_mag;
    }

    float KP   = 0.10;
    float Pdis = 0;

    ans.x = speed;
    if (std::fabs(diff.x) < Pdis) // always FALSE (?)
    {
        ans.x = KP * max_dec.x * std::fabs(diff.x);
    }
    else
    {
        ans.x = pow(0.6f * max_dec.x * std::fabs(diff.x), 0.6f);
    }
    ans.x *= Common::sign(diff.x);
    if (std::fabs(diff.x) < 5)
        ans.x = 0.0f;

    if (ans.x * oldAns[state.vision_id].x <= 0)
    {
        float tmp = oldAns[state.vision_id].x + max_dec.x * Common::sign(ans.x);
        if (ans.x == 0)
            tmp = oldAns[state.vision_id].x - max_dec.x * Common::sign(oldAns[state.vision_id].x);

        if (tmp * ans.x > 0)
        {
            tmp = std::min(max_acc.x, std::fabs(tmp)) * Common::sign(ans.x);
            if (std::fabs(tmp) > std::fabs(ans.x))
                tmp = ans.x;
        }

        ans.x = tmp;
    }
    else
    {
        if (std::fabs(ans.x) > std::fabs(oldAns[state.vision_id].x) + max_acc.x)
        {
            ans.x = (std::fabs(oldAns[state.vision_id].x) + max_acc.x) * Common::sign(ans.x);
        }
        else if (std::fabs(ans.x) < std::fabs(oldAns[state.vision_id].x) - max_dec.x)
        {
            ans.x = (std::fabs(oldAns[state.vision_id].x) - max_dec.x) * Common::sign(ans.x);
        }
        if (std::fabs(ans.x) > tmp_max_speed.x)
        {
            if (Common::sign(ans.x) == 0)
            { // NOT ENTERING HERE!!!!
                ans.x = std::max(std::fabs(oldAns[state.vision_id].x) - max_dec.x, std::fabs(tmp_max_speed.x)) *
                        Common::sign(oldAns[state.vision_id].x);
            }
            else
                ans.x = std::max(std::fabs(oldAns[state.vision_id].x) - max_dec.x, std::fabs(tmp_max_speed.x)) *
                        Common::sign(ans.x);
        }
    }

    ans.y = speed;
    if (std::fabs(diff.y) < Pdis)
    {
        ans.y = KP * max_dec.y * std::fabs(diff.y);
    }
    else
    {
        ans.y = pow(0.6f * max_dec.y * std::fabs(diff.y), 0.6f);
    }
    ans.y *= Common::sign(diff.y);
    if (std::fabs(diff.y) < 5)
    {
        ans.y = 0; // std::max(0,std::fabs(oldAns[state.vision_id].y)-max_dec.y)*Common::sign(ans.y);
    }
    if (ans.y * oldAns[state.vision_id].y <= 0)
    {
        float tmp = oldAns[state.vision_id].y + max_dec.y * Common::sign(ans.y);
        if (ans.y == 0)
            tmp = oldAns[state.vision_id].y - max_dec.y * Common::sign(oldAns[state.vision_id].y);
        if (tmp * ans.y > 0)
        {
            tmp = std::min(max_acc.y, std::fabs(tmp)) * Common::sign(ans.y);
            if (std::fabs(tmp) > std::fabs(ans.y))
                tmp = ans.y;
        }

        ans.y = tmp;
    }
    else
    {
        if (std::fabs(ans.y) > std::fabs(oldAns[state.vision_id].y) + max_acc.y)
        {
            ans.y = (std::fabs(oldAns[state.vision_id].y) + max_acc.y) * Common::sign(ans.y);
        }
        else if (std::fabs(ans.y) < std::fabs(oldAns[state.vision_id].y) - max_dec.y)
        {
            ans.y = (std::fabs(oldAns[state.vision_id].y) - max_dec.y) * Common::sign(ans.y);
        }
        if (std::fabs(ans.y) > tmp_max_speed.y)
        {
            if (Common::sign(ans.y) == 0)
                ans.y = std::max(std::fabs(oldAns[state.vision_id].y) - max_dec.y, std::fabs(tmp_max_speed.y)) *
                        Common::sign(oldAns[state.vision_id].y);
            else
                ans.y = std::max(std::fabs(oldAns[state.vision_id].y) - max_dec.y, std::fabs(tmp_max_speed.y)) *
                        Common::sign(ans.y);
        }

        if (std::fabs(ans.y) > std::fabs(oldAns[state.vision_id].y))
            if (std::fabs(ans.y - oldAns[state.vision_id].y) > max_acc.y * 1.1)
                std::cout << "	gaz nade	" << max_acc.y << "		<	"
                          << std::fabs(ans.y - oldAns[state.vision_id].y);

        if (std::fabs(ans.y) < std::fabs(oldAns[state.vision_id].y))
            if (std::fabs(ans.y - oldAns[state.vision_id].y) > max_dec.y * 1.1)
                std::cout << "	tormoz nakon	" << max_dec.y << "		<	"
                          << std::fabs(ans.y - oldAns[state.vision_id].y);

        std::cout << std::endl;
    }

    oldAns[state.vision_id] = ans;

    return ans;
}
} // namespace Tyr::Soccer