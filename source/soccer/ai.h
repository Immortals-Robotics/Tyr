#pragma once

#include "dss/dss.h"
#include "errt/errt.h"
#include "helpers/one_touch_detector.h"
#include "plays/play_book.h"

namespace Tyr::Soccer
{
class Ai
{
private:
    Common::WorldState   m_world_state;
    Common::RefereeState m_ref_state;

    std::unique_ptr<Common::NngClient> m_world_client;
    std::unique_ptr<Common::NngClient> m_ref_client;

    std::unique_ptr<Common::UdpClient> m_strategy_client;

    std::unique_ptr<Common::NngServer> m_cmd_server;

    Common::Random m_random;

    using Play = void (Ai::*)();
    Play currentPlay;

    int FUNC_state = 0;
    int FUNC_CNT   = 0;

    bool gkIntercepting;

    Common::Angle chip_head;

    float randomParam;
    int   target_str;

    int gk     = 0;
    int def    = 1;
    int dmf    = 2;
    int mid2   = 3;
    int mid1   = 4;
    int attack = 5;
    int rw     = 6;
    int lw     = 7;

    int *stm2AInum[Common::Setting::kMaxRobots] = {};

    Common::Vec2 allafPos[Common::Setting::kMaxRobots];

    std::map<int *, int> markMap;

    int lastReferee;

    Common::Timer timer;

    bool isDefending;
    bool oppRestarted;

    const int                     maxBallHist = 240;
    std::deque<Common::BallState> ballHist;
    Common::Linear                ballLine;

    Planner planner[Common::Setting::kMaxRobots];
    Dss    *dss;

    OneTouchDetector oneTouchDetector[Common::Setting::kMaxRobots];
    enum OneTouchType
    {
        oneTouch = 0,
        shirje,
        gool,
        allaf
    };
    OneTouchType oneTouchType[Common::Setting::kMaxRobots];
    bool         oneTouchTypeUsed[Common::Setting::kMaxRobots];

    int side;

    // Helpers
    Common::Vec2 CalculatePassPos(int robot_num, const Common::Vec2 &target, const Common::Vec2 &statPos,
                                  float bar = 89.0f);
    void         calculateBallTrajectory();
    float        calculateRobotReachTime(int robot_num, Common::Vec2 dest, VelocityProfile vel_profile);
    float        calculateBallRobotReachTime(int robot_num, VelocityProfile vel_profile);

    // boz ha
    void ManageAttRoles();
    void CalcIsDefending();
    void MarkManager(bool restart = true);

    PlayBook m_playbook;
    int      strategy_weight();

    Common::Vec2  PredictedBall;
    bool          circleReachedBehindBall;
    Common::Angle calculateOneTouchAngle(int robot_num, Common::Vec2 oneTouchPosition);

    struct OpenAngle
    {
        // Center of the angle
        Common::Angle center;
        // how big it is
        Common::Angle magnitude;
    };

    OpenAngle calculateOpenAngleToGoal(Common::Vec2 init, int robot_num);

    bool         ballIsGoaling();
    int          findNearestOpp(Common::Vec2 pos, int mask, bool acceptNearBall = true);
    int          findKickerOpp(int mask, float max_dis = 500.0f);
    bool         goalBlocked(Common::Vec2 init_pos, float max_shoot_blocker_dis, float shoot_blocker_r);
    bool         attackFuckingAngle();
    Common::Vec2 predictBallForwardAI(float timeAhead);
    float        calculateOppThreat(int opp, bool restart = false);
    float        calculateMarkCost(int robot_num, int opp);
    float        calculateSwitchToAttackerScore(int robot_num);

    // Skills
    enum NavigationFlags
    {
        NavigationFlagsNone                    = 0,
        NavigationFlagsForceNoObstacles        = (1 << 1), // only used in ball placement
        NavigationFlagsForceBallObstacle       = (1 << 2), // 500.0f
        NavigationFlagsForceBallMediumObstacle = (1 << 3), // 230.0f
        NavigationFlagsForceBallSmallObstacle  = (1 << 4), // 60.0f
    };

    void navigate(int robot_num, Common::Vec2 dest, VelocityProfile velocityProfile = VelocityProfile::mamooli(),
                  NavigationFlags t_flags = NavigationFlagsNone);
    void setObstacles(int robot_num, NavigationFlags t_flags = NavigationFlagsNone);

    void Mark(int robot_num, int opp, float dist = 220.0f);
    void Mark2Goal(int robot_num, int opp, float dist = 220.0f);
    void Mark2Ball(int robot_num, int opp, float dist = 220.0f);
    void Halt(int robot_num);

    void GK_shirje(int robot_num = 0);
    void GKHi(int robot_num = 0, bool stop = false);
    void DefHi(int robot_num, int rightdef_num, int leftdef_num, Common::Vec2 *defendTarget = nullptr,
               bool stop = false);
    void runningDef(int robot_num, Common::Vec2 target, Common::Vec2 *defendTarget, bool stop);
    void DefenceWall(int robot_num, bool kickOff = false);
    void attacker(int robot_num, Common::Angle angle, int kick = 0, int chip = 0, bool gameRestart = false,
                     bool kiss = false, bool dribbler = false);
    void WaitForPass(int robot_num, bool chip = false, const Common::Vec2 *target = nullptr,
                     Common::Vec2 *statPos = nullptr);
    void WaitForOmghi(int robot_num, bool chip = false);
    void WaitForGool(int robot_num, bool chip = false);
    void receivePass(int robot_num, Common::Vec2 staticPos, bool chip = false);
    void circle_ball(int robot_num, Common::Angle tagret_angle, int shoot_pow, int chip_pow, float precision,
                     float near_dis_override = -1.0f);

    // Plays
    void Stop();
    void Stop_def();
    void NewNormalPlay();
    void NormalPlayDef();
    void NormalPlayAtt();
    void HaltAll();
    void penalty_us_shootout();
    void kickoff_us_chip();
    void throwin_chip_shoot();
    void kickoff_their_one_wall();
    void penalty_their_simple();
    void corner_their_global();
    void strategy();
    void our_place_ball();
    void their_place_ball();

    void internalProcessData();

    inline Common::Vec2 ownGoal() const
    {
        return Common::Vec2(side * Common::field().width, 0);
    }

    inline Common::Vec2 ownGoalPostTop() const
    {
        return ownGoal() + Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
    }

    inline Common::Vec2 ownGoalPostBottom() const
    {
        return ownGoal() - Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
    }

    inline Common::LineSegment ownGoalLine() const
    {
        return {ownGoalPostBottom(), ownGoalPostTop()};
    }

    inline Common::Vec2 oppGoal() const
    {
        return Common::Vec2(-side * Common::field().width, 0);
    }

    inline Common::Vec2 oppGoalPostTop() const
    {
        return oppGoal() + Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
    }

    inline Common::Vec2 oppGoalPostBottom() const
    {
        return oppGoal() - Common::Vec2{0.0f, Common::field().goal_width / 2.0f};
    }

    inline Common::LineSegment oppGoalLine() const
    {
        return {oppGoalPostBottom(), oppGoalPostTop()};
    }

    inline bool isOut(Common::Vec2 t_point, const float t_margin = 0.0f) const
    {
        return std::fabs(t_point.x) > Common::field().width + t_margin ||
               std::fabs(t_point.y) > Common::field().height + t_margin;
    }

public:
    Robot OwnRobot[Common::Setting::kMaxRobots];

    Ai();

    bool receiveWorld();
    bool receiveReferee();

    bool publishCommands() const;

    void process();

    bool receivePlayBook();
    bool loadPlayBook(const std::filesystem::path &t_path);
    bool setPlayBook(const Protos::Immortals::PlayBook &t_playbook);
};
} // namespace Tyr::Soccer
