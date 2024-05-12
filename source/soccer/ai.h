#pragma once

#include "dss/dss.h"
#include "errt/errt.h"
#include "helpers/one_touch_detector.h"

#define mark_in_stop 0

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

    std::map<std::string, void (Ai::*)()> AIPlayBook;
    std::string                           currentPlay;

    void InitAIPlayBook();

    int FUNC_state = 0;
    int FUNC_CNT   = 0;

    bool gkIntercepting;

    Common::Angle chip_head;

    float randomParam;
    int   target_str;

    int gk  = 3;
    int def = 5;
    int dmf = 4;
    int rmf = 1;
    int lmf = 2;
    int cmf = 0;
    int rw  = 6;
    int lw  = 7;

    int attack;
    int mid1;
    int mid2;

    int *stm2AInum[Common::Setting::kMaxOnFieldTeamRobots];

    Common::Vec2 allafPos[Common::Setting::kMaxOnFieldTeamRobots];

    std::map<int *, int> markMap;

    int lastReferee;

    Common::Timer timer;

    bool isDefending;
    bool oppRestarted;

    float beta;
    float gamma;
    float shootK;

    const int                     maxBallHist = 240;
    std::deque<Common::BallState> ballHist;
    Common::Linear                ballLine;

    Planner planner[Common::Setting::kMaxOnFieldTeamRobots];
    Dss    *dss;

    OneTouchDetector oneTouchDetector[Common::Setting::kMaxOnFieldTeamRobots];
    enum OneTouchType
    {
        oneTouch = 0,
        shirje,
        gool,
        allaf
    };
    OneTouchType oneTouchType[Common::Setting::kMaxOnFieldTeamRobots];
    bool         oneTouchTypeUsed[Common::Setting::kMaxOnFieldTeamRobots];

    bool navigated[Common::Setting::kMaxOnFieldTeamRobots];
    int  side;

    // Helpers
    Common::Vec2 DefGhuz(Common::Vec2 *defendTarget = nullptr);
    Common::Vec2 CalculatePassPos(int robot_num, const Common::Vec2 &target, const Common::Vec2 &statPos,
                                  float bar = 89.0f);
    void         calculateBallTrajectory();
    float        calculateRobotReachTime(int robot_num, Common::Vec2 dest, VelocityProfile::Type vel_profile);
    float        calculateBallRobotReachTime(int robot_num, VelocityProfile::Type vel_profile);

    // boz ha
    void ManageAttRoles();
    void CalcIsDefending();
    void MarkManager(bool restart = true);

    Protos::Immortals::PlayBook m_playbook;
    int                         strategy_weight();

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
    int          findNearestAsshole(Common::Vec2 pos, int mask, bool acceptNearBall = true);
    int          findKickerOpp(int mask, float max_dis = 500.0f);
    bool         goalBlocked(Common::Vec2 init_pos, float max_shoot_blocker_dis, float shoot_blocker_r);
    bool         attackFuckingAngle();
    Common::Vec2 predictBallForwardAI(float timeAhead);
    float        calculateOppThreat(int opp, bool restart = false);
    float        calculateMarkCost(int robot_num, int opp);
    float        calculateSwitchToAttackerScore(int robot_num);
    float        outOfField(Common::Vec2 point);

    // These functions make sure the required robots are present (in case if any of the robots got out):
    void wantThisRobot(int robot_num); // First we tell which robot we need
    bool positionRobots(bool avoid_GK  = true,
                        bool avoid_DEF = true); // now we swap the apsent robots (TRUE if it succeeds)
    bool requiredRobots[Common::Setting::kMaxOnFieldTeamRobots];

    // Skills
    void Navigate2Point(int robot_num, Common::Vec2 dest, float speed = 80.0f,
                        VelocityProfile::Type velocityProfile = VelocityProfile::Type::Mamooli, bool use_dss = false);
    void ERRTNavigate2Point(int robot_num, Common::Vec2 dest, float speed = 80.0f,
                            VelocityProfile::Type velocityProfile = VelocityProfile::Type::Mamooli);
    void ERRTSetObstacles(int robot_num, bool bll = false, bool field = true);
    void ERRTSetGkClearObstacles(int robot_num);
    void Mark(int robot_num, int opp, float dist = 220.0f);
    void Mark2Goal(int robot_num, int opp, float dist = 220.0f);
    void Mark2Ball(int robot_num, int opp, float dist = 220.0f);
    void Halt(int robot_num);
    void GK(int robot_num = 0, int defence_num = 2, bool stop = false);
    void GK_shirje(int robot_num = 0);
    void GKHi(int robot_num = 0, bool stop = false);
    void DefHi(int robot_num, Common::Vec2 *defendTarget = nullptr, bool stop = false);
    void DefenceWall(int robot_num, bool kickOff = false);
    void tech_circle(int robot_num, Common::Angle angle, int kick = 0, int chip = 0, bool needRRT = true,
                     bool gameRestart = false, bool kiss = false, bool dribbler = false, bool needOppRRT = false);
    void WaitForPass(int robot_num, bool chip = false, const Common::Vec2 *target = nullptr,
                     Common::Vec2 *statPos = nullptr);
    void WaitForOmghi(int robot_num, bool chip = false);
    void WaitForGool(int robot_num, bool chip = false);
    void receivePass(int robot_num, Common::Vec2 staticPos, bool chip = false);
    void circle_ball(int robot_num, Common::Angle tagret_angle, int shoot_pow, int chip_pow, float precision,
                     float near_dis_override = -1.0f);
    void circle_ball_free(int robot_num, Common::Angle tagret_angle, int shoot_pow, int chip_pow, float precision,
                          float near_dis_override = -1.0f);

    void DefMid(int &robot_num, int &rightdef_num, int &leftdef_num, Common::Vec2 *defendTarget = nullptr,
                bool stop = false, bool replace = true);
    void DefBy3(int robot_num, int rightdef_num, int leftdef_num, Common::Vec2 *defendTarget = nullptr,
                bool stop = false);
    void DefBy2(int rightdef_num, int leftdef_num, Common::Vec2 *defendTarget = nullptr, bool stop = false);
    void DefBy1(int thelastdef_num, Common::Vec2 *defendTarget = nullptr, bool stop = false);
    void runningDef(int robot_num, Common::Vec2 target, Common::Vec2 *defendTarget, bool stop);

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
    void strategy_maker();
    void our_place_ball_shoot();
    void our_place_ball_shoot_V2();
    void our_place_ball_shoot_taki();
    void their_place_ball();

    void internalPrepare();
    void internalProcessData();

public:
    Robot OwnRobot[Common::Setting::kMaxOnFieldTeamRobots];
    Ai();

    bool receiveWorld();
    bool receiveReferee();

    bool publishCommands() const;

    void process();

    bool receivePlayBook();
    bool loadPlayBook(const std::filesystem::path &t_path);
    bool setPlayBook(const Protos::Immortals::PlayBook& t_playbook);
};
} // namespace Tyr::Soccer
