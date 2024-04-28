#pragma once

#include "dss/dss.h"
#include "errt/errt.h"
#include "helpers/one_touch_detector.h"

#define NEW_FIELD_2018

#define mark_in_stop 0

namespace Tyr::Soccer
{
class Ai
{
private:
    Common::WorldState   *worldState;
    Common::RefereeState *refereeState;
    Sender::Sender       *senderBase;
    Common::GameState    *REF_playState;

    Common::Random random;

    float field_width;
    float field_height;
    float goal_width;

    float penalty_area_r;
    float penalty_area_width;

    std::map<std::string, void (Ai::*)()> AIPlayBook;
    std::string                           currentPlay;
    uint32_t                              currentPlayParam;

    void InitAIPlayBook();

    int FUNC_state = 0;
    int FUNC_CNT   = 0;

    bool gkIntercepting;

    Common::Angle chip_head;

    Common::Angle penaltyAngle;

    int oppGK;

    float randomParam;
    int   target_str;

    int rmf;
    int gk;
    int def;
    int dmf;
    int lmf;
    int cmf;
    int rw;
    int lw;

    int attack;
    int mid1;
    int mid2;

    int *stm2AInum[Common::Setting::kMaxOnFieldTeamRobots];

    Common::Vec2 allafPos[Common::Setting::kMaxOnFieldTeamRobots];

    std::map<int *, int> markMap;

    int lastReferee;

    Common::Timer timer;
    Common::Vec2 *targetBallPlacement;

    bool isDefending;
    bool oppRestarted;

    float beta;
    float gamma;
    float shootK;

    bool reached;

    const int                     maxBallHist;
    std::deque<Common::BallState> ballHist;
    Common::Linear                ballLine;
    Common::BallState             ball;
    Common::RobotState            OppRobot[Common::Setting::kMaxRobots];
    int                           OwnRobotNum, OppRobotNum;
    Planner                       planner[Common::Setting::kMaxOnFieldTeamRobots];
    Dss                          *dss;

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

    VelocityProfile BALL_PLACE_KHEYLI_SOOSKI;
    VelocityProfile VELOCITY_PROFILE_AROOM;
    VelocityProfile VELOCITY_PROFILE_KHARAKI;
    VelocityProfile VELOCITY_PROFILE_MAMOOLI;

    // Helpers
    Common::Vec2 GK_Ghuz(float predictBallT, float rMul, int def_count = 2);
    Common::Vec2 GK_Ghuz_2018(float predictBallT, float rMul, int def_count = 2);
    Common::Vec2 DefGhuz(Common::Vec2 *defendTarget = NULL);
    Common::Vec2 CalculatePassPos(int robot_num, const Common::Vec2 &target, const Common::Vec2 &statPos,
                                  float bar = 89.0f);
    void         CalculateBallTrajectory();
    float        calculateRobotReachTime(int robot_num, Common::Vec2 dest, VelocityProfile *vel_profile);
    float        calculateBallRobotReachTime(int robot_num, VelocityProfile *vel_profile);

    // boz ha
    void ManageAttRoles();
    void CalcIsDefending();
    void MarkManager(bool restart = true);

    Protos::Immortals::PlayBook *playBook;
    int                          strategy_weight();

    Common::Vec2  lastBallVelocity;
    Common::Vec2  PredictedBall;
    bool          circleReachedBehindBall;
    Common::Angle calculateOneTouchAngle(int robot_num, Common::Vec2 oneTouchPosition);

    void SetEvaulateOBJs(int robot_num1, int robot_num2);
    void PassEvaluation();

    Common::Vec2 calculateOpenAngleToGoal(Common::Vec2 init, int robot_num);

    bool         ballIsGoaling();
    int          findNearestAsshole(Common::Vec2 pos, int mask, bool acceptNearBall = true);
    int          findCruncherOpp(int mask1, int mask2 = -1, bool acceptNearBall = false);
    int          findKickerOpp(int mask, float max_dis = 500.0f);
    int          findGusheRobot(int mask);
    int          findJeloOpps(float minX, int *ans, int mask1, int mask2, bool acceptNearBall, bool acceptGooshe);
    bool         isGooshe(int id, bool sameSideAsBall);
    bool         goal_blocked(Common::Vec2 init_pos, float max_shoot_blocker_dis, float shoot_blocker_r);
    bool         attackFuckingAngle();
    Common::Vec2 predictBallForwardAI(float timeAhead);
    float        oneTouchScore(int robot_num);
    float        calculateOppThreat(int opp, bool restart = false);
    float        calculateMarkCost(int robot_num, int opp);
    float        calculateSwicthToAttackerScore(int robot_num);
    float        outOfField(Common::Vec2 point);

    // These functions make sure the required robots are present (in case if any of the robots got out):
    void want_this_robot(int robot_num); // First we tell which robot we need
    bool position_robots(bool avoid_GK  = true,
                         bool avoid_DEF = true); // now we swap the apsent robots (TRUE if it succeeds)
    bool requiredRobots[Common::Setting::kMaxOnFieldTeamRobots];

    // Skills
    void Navigate2Point(int robot_num, Common::Vec2 dest, bool accurate = false, int speed = 80,
                        VelocityProfile *velocityProfile = NULL, bool use_dss = false);
    void Navigate2Point_2018(int robot_num, Common::Vec2 dest, int speed = 80, VelocityProfile *velocityProfile = NULL);
    void ERRTNavigate2Point(int robot_num, Common::Vec2 dest, bool accurate = false, int speed = 80,
                            VelocityProfile *velocityProfile = NULL);
    void ERRTSetObstacles(int robot_num, bool bll = false, bool field = true);
    void ERRTSetGkClearObstacles(int robot_num);
    void Mark(int robot_num, int opp, float dist = 220.0f);
    void Mark2Goal(int robot_num, int opp, float dist = 220.0f);
    void Mark2Ball(int robot_num, int opp, float dist = 220.0f);
    void Halt(int robot_num);
    void GK(int robot_num = 0, int defence_num = 2, bool stop = false);
    void GK_shirje(int robot_num = 0);
    void GK_shirje_2018(int robot_num, VelocityProfile *VELOCITY_PROFILE);
    void GKHi(int robot_num = 0, bool stop = false);
    void GKHi_Simple(int robot_num = 0, bool stop = false);
    void OneDef(int robot_num = 1, Common::Vec2 *defendTarget = NULL, bool stop = false);
    void TwoDef(int robot_num1 = 1, int robot_num2 = 2, Common::Vec2 *defendTarget = NULL);
    void DefHi(int robot_num, Common::Vec2 *defendTarget = NULL, bool stop = false);
    void PenaltyUs(int robot_num, Common::Angle angle, int kick = 0, int chip = 0);
    void DefenceWall(int robot_num, bool kickOff = false);
    void tech_circle(int robot_num, Common::Angle angle, int kick = 0, int chip = 0, bool needRRT = true,
                     bool gameRestart = false, bool kiss = false, bool dribbler = false, bool needOppRRT = false);
    void intercept_ball(int robot_num, Common::Angle angle, int shoot_pow, int chip_pow);
    void WaitForPass(int robot_num, bool chip = false, Common::Vec2 *target = NULL, Common::Vec2 *statPos = NULL);
    void WaitForOmghi(int robot_num, bool chip = false);
    void WaitForGool(int robot_num, bool chip = false);
    void recievePass(int robot_num, Common::Vec2 staticPos, bool chip = false);
    void backPass(int robot_num, float target, float t);
    void dribble(int robot_num, Common::Vec2 target);
    void circle_ball(int robot_num, Common::Angle tagret_angle, int shoot_pow, int chip_pow, float precision,
                     float near_dis_override = -1.0f);
    void circle_ball_free(int robot_num, Common::Angle tagret_angle, int shoot_pow, int chip_pow, float precision,
                          float near_dis_override = -1.0f);
    void circle_ball_free_V2(int robot_num, Common::Angle tagret_angle, int shoot_pow, int chip_pow, float precision,
                             VelocityProfile temp_vel, float near_dis_override = -1.0f);

    void DefMid(int &robot_num, int &rightdef_num, int &leftdef_num, Common::Vec2 *defendTarget = NULL,
                bool stop = false, bool replace = true);
    void DefBy3(int robot_num, int rightdef_num, int leftdef_num, Common::Vec2 *defendTarget = NULL, bool stop = false);
    void DefBy2(int rightdef_num, int leftdef_num, Common::Vec2 *defendTarget = NULL, bool stop = false);
    void DefBy1(int thelastdef_num, Common::Vec2 *defendTarget = NULL, bool stop = false);
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
    void tech_challenge();
    void tech_mexico();
    void tech_cmu();
    void tech_khers_pass();
    void tech_khers_def();
    void tech_motion_ann();
    void our_place_ball_shoot();
    void our_place_ball_shoot_V2();
    void our_place_ball_shoot_taki();
    void their_place_ball();
    void far_penalty_shoot();

    void my_test();

    void internalProcessData(Common::WorldState *worldState);
    void internalFinalize(Common::WorldState *worldState);

public:
    Robot OwnRobot[Common::Setting::kMaxOnFieldTeamRobots];
    Ai(Common::WorldState *worldState, Common::RefereeState *refereeState, Sender::Sender *sender);
    void Process(Common::WorldState *worldState);
    bool read_playBook(const char *fileName);
    bool read_playBook_str(char *buffer, int length);

protected:
    bool debugDraw;
    void AddDebugPoint(const Common::Vec2 &p, const Common::CommonColor _color = Common::White);
    void AddDebugLine(const Common::Vec2 &p1, const Common::Vec2 &p2, const Common::CommonColor _color = Common::White);
    void AddDebugRect(const Common::Vec2 &p, const float w, const float h,
                      const Common::CommonColor _color = Common::White);
    void AddDebugCircle(const Common::Vec2 &p, const float r, const Common::CommonColor _color = Common::White);
};
} // namespace Tyr::Soccer
