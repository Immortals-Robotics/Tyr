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

Ai::Ai(std::vector<std::unique_ptr<Sender::ISender>> &senders)
{
    Common::logInfo("Running Immortals SSL AI module");
    Common::logInfo("Hope us luck :D ");

    for (auto &sender : senders)
        m_senders.push_back(sender.get());

    dss = new Dss(OwnRobot, Common::worldState().opp_robot, 1.f / 61.57f, 7000.f, 3000.f);

    InitAIPlayBook();
    currentPlay = "HaltAll";

    gkIntercepting = false;

    randomParam = 0.0f;
    target_str  = -1;

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

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        oneTouchDetector[i].rState = &OwnRobot[i];
        oneTouchDetector[i].side   = &side;

        oneTouchType[i]     = oneTouch;
        oneTouchTypeUsed[i] = false;

        allafPos[i] = Common::Vec2(0, 0);
    }

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        OwnRobot[i].setVisionId(i + 1);
    }

#if 0
    OwnRobot[gk].setVisionId(4);
    OwnRobot[def].setVisionId(10);
    OwnRobot[dmf].setVisionId(1);
    OwnRobot[lmf].setVisionId(6);
    OwnRobot[rmf].setVisionId(0);
    OwnRobot[cmf].setVisionId(2);
    OwnRobot[rw].setVisionId(7);
    OwnRobot[lw].setVisionId(8);
#else
    // TODO comment this (used fogrSim)
    OwnRobot[gk].setVisionId(0);
    OwnRobot[def].setVisionId(1);
    OwnRobot[dmf].setVisionId(2);
    OwnRobot[lmf].setVisionId(3);
    OwnRobot[rmf].setVisionId(4);
    OwnRobot[cmf].setVisionId(5);
    OwnRobot[rw].setVisionId(6);
    OwnRobot[lw].setVisionId(7);
#endif

    chip_head = Common::Angle::fromDeg(200);

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
        Common::logInfo("Strategy loaded with size {}", playBook->strategy_size());
        Common::logDebug("{} {}", playBook->strategy_size(), playBook->strategy(0).name());

        Common::logDebug("{}", playBook->strategy_size());
        for (int i = 0; i < playBook->strategy_size(); i++)
        {
            Common::logDebug("    {}", playBook->strategy(i).role_size());
            for (int j = 0; j < playBook->strategy(i).role_size(); j++)
            {
                Common::logDebug("        {}", playBook->strategy(i).role(j).path_size());
                for (int k = 0; k < playBook->strategy(i).role(j).path_size(); k++)
                {
                    Common::logDebug("            {}", (int) playBook->strategy(i).role(j).path(k).type());
                    Common::logDebug("                {}        {}", playBook->strategy(i).role(j).path(k).x(),
                                     playBook->strategy(i).role(j).path(k).y());
                }
            }
        }

        Common::logDebug("{}", playBook->weight_size());
        for (int i = 0; i < playBook->weight_size(); i++)
        {
            Common::logDebug("    {}", playBook->weight(i));
        }
    }
    else
    {
        Common::logCritical("Could not open \"strategy.ims\"");
    }

    timer.start();
}
} // namespace Tyr::Soccer
