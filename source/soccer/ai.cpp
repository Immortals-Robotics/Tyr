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

Ai::Ai()
{
    Common::logInfo("Running Immortals SSL AI module");
    Common::logInfo("Hope us luck :D ");

    m_world_client = std::make_unique<Common::NngClient>(Common::setting().world_state_url);
    m_ref_client   = std::make_unique<Common::NngClient>(Common::setting().referee_state_url);

    m_strategy_client = std::make_unique<Common::UdpClient>(Common::setting().strategy_address);

    m_cmd_server = std::make_unique<Common::NngServer>(Common::setting().commands_url);

    dss = new Dss(OwnRobot, m_world_state.opp_robot, 1.f / 61.57f, 7000.f, 3000.f);

    currentPlay = &Ai::HaltAll;

    gkIntercepting = false;

    randomParam = 0.0f;
    target_str  = -1;

    isDefending  = false;
    oppRestarted = false;

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

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        OwnRobot[i] = Robot(&m_world_state.own_robot[i]);

        oneTouchDetector[i].rState = &OwnRobot[i];
        oneTouchDetector[i].ball   = &m_world_state.ball;
        oneTouchDetector[i].side   = &side;

        oneTouchType[i]     = oneTouch;
        oneTouchTypeUsed[i] = false;

        allafPos[i] = Common::Vec2(0, 0);
    }

    chip_head = Common::Angle::fromDeg(200);

    circleReachedBehindBall = false;
    PredictedBall           = Common::Vec2();

    const auto strategy_path = std::filesystem::path(DATA_DIR) / "strategy.ims";
    loadPlayBook(strategy_path);

    timer.start();
}

bool Ai::receiveWorld()
{
    Protos::Immortals::WorldState pb_state;
    if (!m_world_client->receive(&pb_state, nullptr, true))
        return false;

    m_world_state = Common::WorldState(pb_state);
    return true;
}

bool Ai::receiveReferee()
{
    Protos::Immortals::RefereeState pb_state;
    if (!m_ref_client->receive(&pb_state, nullptr, true))
        return false;

    m_ref_state = Common::RefereeState(pb_state);
    return true;
}

bool Ai::publishCommands() const
{
    const Common::TimePoint time = Common::TimePoint::now();

    Protos::Immortals::CommandsWrapper pb_wrapper;

    pb_wrapper.set_time(time.microseconds());

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        OwnRobot[i].currentCommand().fillProto(pb_wrapper.add_command());
    }

    return m_cmd_server->send(time, pb_wrapper);
}
} // namespace Tyr::Soccer
