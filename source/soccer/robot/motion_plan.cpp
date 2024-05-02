#include "robot.h"

namespace Tyr::Soccer
{
Common::Vec2 vdes_ann;

Common::Vec3 Robot::MotionPlan(Common::RobotState state, Common::RobotState target, float speed, bool accurate,
                               VelocityProfile *velocityProfile)
{
    /*Common::Vec2 max_spd = Common::Vec2 ( 100.0f );
    Common::Vec2 max_dec = Common::Vec2 ( 2.3f );
    Common::Vec2 max_acc = Common::Vec2 ( 1.6f );
    float max_w_acc = 40.0f;
    float max_w_dec = 140.0f;*/

    Common::Vec2 max_spd   = velocityProfile->max_spd;
    Common::Vec2 max_dec   = velocityProfile->max_dec;
    Common::Vec2 max_acc   = velocityProfile->max_acc;
    float        max_w_acc = velocityProfile->max_w_acc;
    float        max_w_dec = velocityProfile->max_w_dec;

    //    if (oldRobot)
    //    {
    //        max_spd = Common::Vec2(100.0f);
    //        max_dec = Common::Vec2(5.2f);
    //        max_acc = Common::Vec2(2.6f);
    //        max_w_acc = 15.0f;
    //        max_w_dec = 200.0f;
    //    }

    static Common::Vec3 oldAns[12] = {Common::Vec3(0.0f), Common::Vec3(0.0f), Common::Vec3(0.0f), Common::Vec3(0.0f),
                                      Common::Vec3(0.0f), Common::Vec3(0.0f), Common::Vec3(0.0f), Common::Vec3(0.0f),
                                      Common::Vec3(0.0f), Common::Vec3(0.0f), Common::Vec3(0.0f), Common::Vec3(0.0f)};

    Common::Vec3 ans; // The output of this function
    ans.z = target.angle.deg();
    while (ans.z > 180)
    {
        ans.z -= 360;
    }
    while (ans.z < -180)
    {
        ans.z += 360;
    }
    ans.z /= 5.0f;
    //    if (oldRobot)
    //    {
    //
    //        //ans.z *= 3.0f;
    //
    //        ans.z = sqrt(2.0f * max_w_dec * std::fabs(ans.z)) * Common::sign(ans.z);
    //
    //        //ans.x += Vel_offset.x;
    //        if (ans.z * oldAns[state.vision_id].z < 0)
    //        {
    //            float tmp = oldAns[state.vision_id].z + max_w_dec * Common::sign(ans.z);
    //            //float tmp = oldAns[state.vision_id].x + 20.0f * max_acc.x * sgn ( ans.x );
    //
    //            if (tmp * ans.z > 0)
    //            {
    //                tmp = max_w_acc * Common::sign(ans.z);
    //                if (std::fabs(tmp) > std::fabs(ans.z))
    //                    tmp = ans.z;
    //            }
    //
    //            ans.z = tmp;
    //        } else
    //        {
    //            if (std::fabs(ans.z) > std::fabs(oldAns[state.vision_id].z) + max_w_acc)
    //            {
    //                ans.z = (std::fabs(oldAns[state.vision_id].z) + max_w_acc) * Common::sign(ans.z);
    //            }
    //        }
    //        if (std::fabs(ans.z) > 180)
    //        {
    //            ans.z = 180.0f * Common::sign(ans.z);
    //        }
    //
    //        if (ans.z > 180)
    //            ans.z = 180;
    //        if (ans.z < -180)
    //            ans.z = -180;
    //    }

    target.position = target.position - state.position;

    /*float trans_rad = ( 90.0f - state.angle ) * ( 3.1415f / 180.0f );
     target.position = Common::Vec2 (
     cos(trans_rad)*target.position.x - sin(trans_rad)*target.position.y ,
     sin(trans_rad)*target.position.x + cos(trans_rad)*target.position.y );*/

    Common::Vec2 tmp_max_speed;

    /*if ( std::fabs(target.position.x) > std::fabs(target.position.y) )
     {
     tmp_max_speed.x = max_spd.x;
     tmp_max_speed.y = max_spd.x * std::fabs(target.position.y)/std::fabs(target.position.x);

     tmp_max_speed.y = std::max ( 0 , std::min ( tmp_max_speed.y , max_spd.y ) );
     }
     else
     {
     tmp_max_speed.y = max_spd.y;
     tmp_max_speed.x = max_spd.y * std::fabs(target.position.x)/std::fabs(target.position.y);

     tmp_max_speed.x = std::max ( 0 , std::min ( tmp_max_speed.x , max_spd.x ) );
     }*/

    float target_dis = Common::Vec2(0.0f).distanceTo(target.position);
    tmp_max_speed.x  = max_spd.x * std::fabs(target.position.x) / target_dis;
    // max_acc.x *= std::fabs(target.position.x) / target_dis;
    // max_dec.x *= std::fabs(target.position.x) / target_dis;

    tmp_max_speed.y = max_spd.y * std::fabs(target.position.y) / target_dis;
    // max_acc.y *= std::fabs(target.position.y) / target_dis;
    // max_dec.y *= std::fabs(target.position.y) / target_dis;

    float tmp_vel_mag = Common::Vec2(0.0f).distanceTo(tmp_max_speed);

    if (tmp_vel_mag > speed)
    {
        tmp_max_speed.x *= speed / tmp_vel_mag;
        tmp_max_speed.y *= speed / tmp_vel_mag;
    }

    // tmp_max_speed = max_spd;

    /*if ( std::fabs ( target.velocity.x ) > 100 )
     target.velocity.x = 100.0f * sgn ( target.velocity.x );
     if ( std::fabs ( target.velocity.y ) > 100 )
     target.velocity.y = 100.0f * sgn ( target.velocity.y );

     Common::Vec2 Vel_offset = Common::Vec2 ( 0.0f );
     float target_dis = DIS ( Common::Vec2 ( 0.0f ) , target.position );
     target_dis = sqrt ( target_dis * 5.0f * DIS ( Common::Vec2 ( 0.0f ) , max_dec ) );

     if ( target_dis < 100 )
     {
     target_dis = 100.0f - target_dis;
     target_dis /= 100.0f;

     Vel_offset.x = target_dis * target.velocity.x;
     Vel_offset.y = target_dis * target.velocity.y;
     }*/

    //    Common::Vec2 LocalVel;
    //    /*LocalVel.x = cos(trans_rad)*state.velocity.x - sin(trans_rad)*state.velocity.y;
    //     LocalVel.y = sin(trans_rad)*state.velocity.x + cos(trans_rad)*state.velocity.y;*/
    //    LocalVel = Common::Vec2(state.velocity.x, state.velocity.y);
    //
    //    LocalVel.x = (LocalVel.x) / 45.0f;
    //    LocalVel.y = (LocalVel.y) / 45.0f;

    /*if ( std::fabs ( oldAns[state.vision_id].x - LocalVel.x ) > 40.0f )
        oldAns[state.vision_id].x = ( oldAns[state.vision_id].x + LocalVel.x ) / 2.0f;
    if ( std::fabs ( oldAns[state.vision_id].y - LocalVel.y ) > 40.0f )
        oldAns[state.vision_id].y = ( oldAns[state.vision_id].y + LocalVel.y ) / 2.0f;*/

    float KP   = 0.10;
    float Pdis = 0;

    // if ( accurate )
    ans.x = speed;
    // else
    if (std::fabs(target.position.x) < Pdis) // always FALSE (?)
    {
        ans.x = KP * max_dec.x * std::fabs(target.position.x);
    }
    else
    {
        ans.x = pow(0.6f * max_dec.x * std::fabs(target.position.x), 0.6f);
    }
    ans.x *= Common::sign(target.position.x);
    // ans.x += Vel_offset.x;
    if (std::fabs(target.position.x) < 5)
        ans.x = 0.0f;

    if (ans.x * oldAns[state.vision_id].x <= 0)
    {
        float tmp = oldAns[state.vision_id].x + max_dec.x * Common::sign(ans.x);
        if (ans.x == 0)
            tmp = oldAns[state.vision_id].x - max_dec.x * Common::sign(oldAns[state.vision_id].x);

        // float tmp = oldAns[state.vision_id].x + 20.0f * max_acc.x * sgn ( ans.x );

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
        /*if ( !accurate )
         {
         if ( 30.0f * oldAns[state.vision_id].x * oldAns[state.vision_id].x / ( 120.0f * max_dec.x ) >
         std::fabs(target.position.x) )
         {
         ans.x = std::max ( 0 , ( std::fabs ( oldAns[state.vision_id].x ) - max_dec.x ) ) * sgn ( ans.x );
         }
         }*/
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
            // ans.x = tmp_max_speed.x * sgn ( ans.x );
        }
    }

    // if ( accurate )
    ans.y = speed;
    // else
    if (std::fabs(target.position.y) < Pdis)
    {
        ans.y = KP * max_dec.y * std::fabs(target.position.y);
    }
    else
    {
        ans.y = pow(0.6f * max_dec.y * std::fabs(target.position.y), 0.6f);
    }
    // ans.y = sqrt ( 2.0f * max_dec.y * std::fabs( target.position.y ) );
    ans.y *= Common::sign(target.position.y);
    // ans.y += Vel_offset.y;
    if (std::fabs(target.position.y) < 5)
    {
        ans.y = 0; // std::max(0,std::fabs(oldAns[state.vision_id].y)-max_dec.y)*Common::sign(ans.y);
    }
    if (ans.y * oldAns[state.vision_id].y <= 0)
    {
        // float tmp = oldAns[state.vision_id].y + 20.0f * max_acc.y * sgn ( ans.y );
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
        /*if ( !accurate )
         {
         if ( 30.0f * oldAns[state.vision_id].y * oldAns[state.vision_id].y / ( 120.0f * max_dec.y ) >
         std::fabs(target.position.y) )
         {
         ans.y = std::max ( 0 , ( std::fabs ( oldAns[state.vision_id].y ) - max_dec.y ) ) * sgn ( ans.y );
         }
         }*/
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

            // ans.y = tmp_max_speed.y * sgn ( ans.y );
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

    // ans.y = target.position.y/20.0;

    // ans.y = 0;

    if (state.vision_id == 1)
        std::cout << (int) ans.z << std::endl;

    /*float kh_mag = DIS ( Common::Vec2 ( 0.0f ) , Common::Vec2 ( ans.x,ans.y ) );
     ans.z *= ( 3000.0f-kh_mag ) / 1500.0f;*/

    if (!accurate)
    {
        // ans = Common::Vec3(vdes_ann.x,vdes_ann.y, 0);
    }

    oldAns[state.vision_id] = ans;

    // if ( state.vision_id == 6 )
    //	std::cout << "	" << ans.x << "	";

    return ans;
}
} // namespace Tyr::Soccer