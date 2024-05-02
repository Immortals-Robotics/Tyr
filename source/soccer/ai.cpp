#include "ai.h"

namespace Tyr::Soccer
{
struct RobotProperty
{
    int  serialID;
    bool oldRobot;
    bool hasKick;
    bool hasChip;
    bool hasDribble;
};

Ai::Ai(Common::WorldState *worldState, Common::RefereeState *refereeState, Sender::Sender *sender)
    : maxBallHist(240), worldState(worldState), refereeState(refereeState), senderBase(sender), cmf(0), rmf(1), lmf(2),
      gk(3), dmf(4), def(5), rw(6), lw(7)
{
    Common::logInfo("Running Immortals SSL AI module");
    Common::logInfo("Hope us luck :D ");

#ifndef NEW_FIELD_2018
    field_width  = 4500.0f;
    field_height = 3000.0f;
    goal_width   = 1000.0f;

    penalty_area_r     = 1000.0f;
    penalty_area_width = 500.0f;
#else
    field_width  = 6000.0f; // The new field is here:
    field_height = 4500.0f;
    goal_width   = 1800.0f; // TODO #6 this was 1000 (!!!) Changed it back

    penalty_area_r     = 1850.0f;
    penalty_area_width = 3650.0f;
#endif

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        oneTouchDetector[i].field_w = field_width;
        oneTouchDetector[i].field_h = field_height;

        planner[i].set_field_params(field_width, field_height);

        OwnRobot[i].field_w = field_width;
        OwnRobot[i].field_h = field_height;
    }

    dss = new Dss(OwnRobot, OppRobot, 92.f, 1.f / 61.57f, 7000.f, 3000.f);

    InitAIPlayBook();
    currentPlay      = "HaltAll";
    currentPlayParam = 0;

    gkIntercepting = false;

    randomParam = 0.0f;
    target_str  = -1;

    reached = false;

    isDefending  = false;
    oppRestarted = false;

    beta   = 0.4;  // Damping factor
    gamma  = 0.14; // Reflect factor
    shootK = 4000.0f;

    lastReferee = Common::RefereeState::STATE_GAME_OFF;

    attack = cmf;
    mid1   = rmf;
    mid2   = lmf;

    markMap[&dmf]  = -1;
    markMap[&mid1] = -1;
    markMap[&mid2] = -1;
    markMap[&lw]   = -1;
    markMap[&rw]   = -1;

    stm2AInum[0] = &gk;
    stm2AInum[1] = &def;
    stm2AInum[2] = &dmf;
    stm2AInum[3] = &mid2;
    stm2AInum[4] = &mid1;
    stm2AInum[5] = &attack;
    stm2AInum[6] = &rw;
    stm2AInum[7] = &lw;
    //    std::cout<<"THE GK_pointer: "<<&gk <<std::endl;
    //    std::cout<<"THE GK_pointer: "<<stm2AInum[0] <<std::endl;

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        oneTouchDetector[i].bState = &ball;
        oneTouchDetector[i].rState = &OwnRobot[i].State;
        oneTouchDetector[i].side   = &side;

        oneTouchType[i]     = oneTouch;
        oneTouchTypeUsed[i] = false;

        allafPos[i] = Common::Vec2(0, 0);
    }

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        OwnRobot[i].set_vision_id(i + 1);
    }

#if 0
    OwnRobot[gk].set_vision_id(4);
    OwnRobot[def].set_vision_id(10);
    OwnRobot[dmf].set_vision_id(1);
    OwnRobot[lmf].set_vision_id(6);
    OwnRobot[rmf].set_vision_id(0);
    OwnRobot[cmf].set_vision_id(2);
    OwnRobot[rw].set_vision_id(7);
    OwnRobot[lw].set_vision_id(8);
#else
    // TODO comment this (used fogrSim)
    OwnRobot[gk].set_vision_id(0);
    OwnRobot[def].set_vision_id(1);
    OwnRobot[dmf].set_vision_id(2);
    OwnRobot[lmf].set_vision_id(3);
    OwnRobot[rmf].set_vision_id(4);
    OwnRobot[cmf].set_vision_id(5);
    OwnRobot[rw].set_vision_id(6);
    OwnRobot[lw].set_vision_id(7);
#endif

    chip_head = Common::Angle::fromDeg(200);

    lastBallVelocity = Common::Vec2();

    circleReachedBehindBall = false;
    PredictedBall           = Common::Vec2();

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
        requiredRobots[i] = false;

    BALL_PLACE_KHEYLI_SOOSKI.max_spd   = Common::Vec2(10.0f);
    BALL_PLACE_KHEYLI_SOOSKI.max_dec   = Common::Vec2(1.0f);
    BALL_PLACE_KHEYLI_SOOSKI.max_acc   = Common::Vec2(0.6f);
    BALL_PLACE_KHEYLI_SOOSKI.max_w_acc = 40.0f;
    BALL_PLACE_KHEYLI_SOOSKI.max_w_dec = 140.0f;

    VELOCITY_PROFILE_AROOM.max_spd   = Common::Vec2(20.0f);
    VELOCITY_PROFILE_AROOM.max_dec   = Common::Vec2(1.0f);
    VELOCITY_PROFILE_AROOM.max_acc   = Common::Vec2(0.8f);
    VELOCITY_PROFILE_AROOM.max_w_acc = 40.0f;
    VELOCITY_PROFILE_AROOM.max_w_dec = 140.0f;

    VELOCITY_PROFILE_MAMOOLI.max_spd   = Common::Vec2(40.0f);
    VELOCITY_PROFILE_MAMOOLI.max_dec   = Common::Vec2(1.5f);
    VELOCITY_PROFILE_MAMOOLI.max_acc   = Common::Vec2(1.0f);
    VELOCITY_PROFILE_MAMOOLI.max_w_acc = 40.0f;
    VELOCITY_PROFILE_MAMOOLI.max_w_dec = 140.0f;

    VELOCITY_PROFILE_KHARAKI.max_spd   = Common::Vec2(40.0f);
    VELOCITY_PROFILE_KHARAKI.max_dec   = Common::Vec2(1.8f);
    VELOCITY_PROFILE_KHARAKI.max_acc   = Common::Vec2(1.3f);
    VELOCITY_PROFILE_KHARAKI.max_w_acc = 40.0f;
    VELOCITY_PROFILE_KHARAKI.max_w_dec = 140.0f;

    playBook = nullptr;
    std::string strategy_path(DATA_DIR);
    strategy_path.append("/strategy.ims");
    read_playBook(strategy_path.c_str());
    if (playBook)
    {
#if 1
        Common::logInfo("Strategy loaded with size {}", playBook->strategy_size());
#else
        std::cout << playBook->strategy_size() << " ";
        std::cout << playBook->strategy(0).name() << std::endl;

        std::cout << playBook->strategy_size() << std::endl;
        for (int i = 0; i < playBook->strategy_size(); i++)
        {
            std::cout << "	" << playBook->strategy(i).role_size() << std::endl;
            for (int j = 0; j < playBook->strategy(i).role_size(); j++)
            {
                std::cout << "		" << playBook->strategy(i).role(j).path_size() << std::endl;
                for (int k = 0; k < playBook->strategy(i).role(j).path_size(); k++)
                {
                    std::cout << "			" << playBook->strategy(i).role(j).path(k).type() << std::endl;
                    std::cout << "				" << playBook->strategy(i).role(j).path(k).x() << "		"
                              << playBook->strategy(i).role(j).path(k).y() << std::endl;
                }
            }
        }

        std::cout << playBook->weight_size() << std::endl;
        for (int i = 0; i < playBook->weight_size(); i++)
        {
            std::cout << "	" << playBook->weight(i) << std::endl;
        }
#endif
    }
    else
    {
        Common::logCritical("Could not open \"strategy.ims\"");
    }

    timer.start();
}
} // namespace Tyr::Soccer
